#include "RPLM.CAD.UI.СonjugationCurves.Command.h"
#include "RPLM.CAD.UI.ConjugationCurves.Resources.h"
#include "RPLM.CAD.ConjugationMethod.h"
#include "RPLM.CAD.Utils.h"
#include "Model/Objects/RGPBodyObject.h"
#include "Generators/BodyConstructor.h"
#include "Model/Representations/RGPModelScene.h"
#include "RPLM.EP.Model/Model/Representations/RGPPresentationContexts.h"
#include "RGPSession.h"

namespace RPLM::CAD
{
	namespace UI
	{
		RPLMCADСonjugationCurvesCommand::RPLMCADСonjugationCurvesCommand() :
			_curveDegree(L"CurveDegree", RSCADUIW("CurveDegree")),
			_controlPointsFilePath(L"ControlPoints", RSCADUIW("ControlPoints"), L""),
			_knotsFilePath(L"Knots", RSCADUIW("Knots"), L""),
			_fixBeginningCurve(L"FixBeginningCurve", RSCADUIW("FixBeginningCurve"), L"", false, true),
			_fixEndCurve(L"FixEndCurve", RSCADUIW("FixEndCurve"), L"", false, true),
			_saveConjugatedCurveInFile(L"SaveConjugatedCurveInFile", RSCADUIW("RPLM.CAD.SaveConjugatedCurveInFile"), L"", false, true),
			_conjugatedCurveFilePath(L"ConjugatedCurveFilePath", RSCADUIW("RPLM.CAD.ConjugatedCurveFilePath"), L"")
		{
			_dialog.SetTitle(RSCADUIW("RPLM.CAD.UI.ConjugationCurves"));

			AddOkToDialog(&_dialog);
			AddCancelToDialog(&_dialog);

			// Степень кривой
			_dialog.AddControl(_curveDegree);

			// Контрольные точки
			_dialog.AddControl(_controlPointsFilePath);
			_controlPointsFilePath.SelectFile();

			// Узловой вектор
			_dialog.AddControl(_knotsFilePath);
			_knotsFilePath.SelectFile();

			// Чекбоксы фиксации начала и конца кривой
			_dialog.AddControl(_fixBeginningCurve);
			_dialog.AddControl(_fixEndCurve);

			// Чекбокс сохранения сопряжённой кривой в файл
			_dialog.AddControl(_saveConjugatedCurveInFile);
			// Путь к файлу для сохранения сопряжённой кривой
			_dialog.AddControl(_conjugatedCurveFilePath);
			_conjugatedCurveFilePath.SetHidden(true);

			_ok.PressEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnOK, this);
			_dialog.OnCloseEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnCloseDialog, this);
			_saveConjugatedCurveInFile.PressEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnSaveConjugatedCurveInFilePressed, this, std::placeholders::_1);
		}

		RPLMCADСonjugationCurvesCommand::~RPLMCADСonjugationCurvesCommand()
		{
		}

		bool RPLMCADСonjugationCurvesCommand::Start(EP::UI::StartCommandParameters& iParameters)
		{
			if (!Command::Start(iParameters))
				return false;

			CreateCommandDialog(_dialog, GetMainWindow(), GetDocument());
			_dialog.NeedToAdjust();
			_dialog.Show();

			return true;
		}

		void RPLMCADСonjugationCurvesCommand::Finish()
		{
			_dialog.Destroy();
			Command::Finish();
		}

		RPLM::EP::UI::ControlLayout* RPLMCADСonjugationCurvesCommand::GetDialog()
		{
			return &_dialog;
		}

		std::string RPLMCADСonjugationCurvesCommand::GetID()
		{
			return "RPLM.CAD.ConjugationCurves";
		}

		void RPLMCADСonjugationCurvesCommand::OnOK()
		{
			// Получаем имена файлов
			Base::Framework::String controlPointsPath = _controlPointsFilePath.GetFullName();
			Base::Framework::String knotsPath = _knotsFilePath.GetFullName();

			const int degree = _curveDegree.GetIntValue();

			if (controlPointsPath.empty() || knotsPath.empty() || degree <= 0)
			{
				return;
			}

			// Считываем данные из файлов
			RGK::Vector<RGK::Math::Vector3D> controlPoints = CAD::Utils::ReadControlPointsFromFile(controlPointsPath);
			Math::Geometry2D::Geometry::DoubleArray knots = CAD::Utils::ReadKnotsFromFile(knotsPath);

			RGK::Context rgkContext;
			EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

			// Создаём объект исходной кривой
			RGK::NURBSCurve curve;
			bool isPeriodic = false;

			if (RGK::NURBSCurve::Create(rgkContext, controlPoints, degree, knots, isPeriodic, curve) != RGK::Result::Success)
			{
				return;
			}

			auto conjugationMethod = std::make_unique<CAD::ConjugationCurves::ConjugationMethod>();
			RGK::NURBSCurve conjugatedCurve = conjugationMethod->ConjugateCurve(curve, _fixBeginningCurve.IsChecked(), _fixEndCurve.IsChecked());

			// Сохраняем в файл, если был активирован чекбокс
			if (_saveConjugatedCurveInFile.IsChecked())
				CAD::Utils::SaveCurveInFile(conjugatedCurve, _conjugatedCurveFilePath.GetFullName());
			
			DrawCurve(conjugatedCurve);

			Terminate();
		}

		bool RPLMCADСonjugationCurvesCommand::OnCloseDialog()
		{
			Terminate();
			return false;
		}

		void RPLMCADСonjugationCurvesCommand::OnSaveConjugatedCurveInFilePressed(EP::UI::ButtonControl& iControl)
		{
			_conjugatedCurveFilePath.SetHidden(!iControl.IsChecked());
			_ok.SetEnabled(IsOkEnabled());
		}

		bool RPLMCADСonjugationCurvesCommand::IsOkEnabled()
		{
			bool isControlPointsFilePathEmpty = _controlPointsFilePath.GetFullName().empty();
			bool isKnotsFilePathEmpty = _knotsFilePath.GetFullName().empty();
			bool isConjugatedCurveFilePathEmpty = false;

			if (_saveConjugatedCurveInFile.IsChecked())
				isConjugatedCurveFilePathEmpty = _conjugatedCurveFilePath.GetFullName().empty();

			return !isControlPointsFilePathEmpty && !isKnotsFilePathEmpty && !isConjugatedCurveFilePathEmpty;
		}

		RGK::Result RPLMCADСonjugationCurvesCommand::DrawCurve(const RGK::NURBSCurve& iCurve) const
		{
			if (!iCurve)
				return RGK::Result::NullPointer;

			RGK::Context rgkContext;
			RPLM::EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

			RGK::BodyConstructor::Data data(0, RGK::Body::Type::Wire);
			data.CreateCoEdgeParametricCurvesAutomatically(true);

			RGK::Math::Vector3DArray controlPoints = iCurve.GetControlPoints();
			std::vector<double> weights = iCurve.GetWeights();
			std::vector<double> knots = iCurve.GetKnots();
			int degree = iCurve.GetDegree();
			bool isPeriodic = iCurve.IsPeriodic();

			const auto tolerance = rgkContext.GetLinearPrecision();
			data.AddVertex(controlPoints.front(), tolerance);
			data.AddVertex(controlPoints.back(), tolerance);

			int ends[2] = { 0, 1 };
			double interval[2] = { knots.front(), knots.back() };
			data.AddEdge(ends, iCurve, true, interval, tolerance);

			RGK::BodyConstructor::Report report;
			auto resultCreationBody = RGK::BodyConstructor::Create(rgkContext, data, report);

			if (resultCreationBody != RGK::Success)
				return resultCreationBody;

			RPLM::EP::Model::ModelScenePtr modelScene = nullptr;

			for (auto i = 0; i < GetDocument()->Representations().Size(); ++i)
			{
				auto presentation = GetDocument()->Representations()[i];

				if (!presentation)
					continue;

				if (presentation->IsTypeOf(RPLM::EP::Model::ModelScene::ClassID()))
				{
					modelScene = EP::Model::Cast<RPLM::EP::Model::ModelScene>(presentation);
					break;
				}
			}

			if (!modelScene)
				return RGK::Result::NullPointer;

			RPLM::EP::Model::EditDocument edit(GetDocument(), RSCADUIW("RPLM.CAD.DrawCurve"));
			{
				auto bodyObject = std::make_shared<RPLM::EP::Model::BodyObject>((report.GetBody()));
				GetDocument()->Objects().AddObject(bodyObject);
				modelScene->EditReferences()->AddObject(bodyObject);
				RPLM::EP::Model::Regeneration::RegenerationContext regenerationContext(GetDocument(), &rgkContext);
				modelScene->Update(RPLM::EP::Model::PresentationUpdateContext(&regenerationContext));
			}

			edit.End(false);
		}
	}
}