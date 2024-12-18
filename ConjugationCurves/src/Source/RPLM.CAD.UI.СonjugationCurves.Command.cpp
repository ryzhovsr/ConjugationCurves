﻿#include "RPLM.CAD.UI.СonjugationCurves.Command.h"
#include "RPLM.CAD.UI.ConjugationCurves.Resources.h"
#include "RPLM.CAD.ConjugationMethod.h"
#include "RPLM.CAD.Utils.h"
#include "Model/Objects/RGPBodyObject.h"
#include "RPLM.EP.Model/Model/Document/RGPMarkContainer.h"
#include "Generators/BodyConstructor.h"
#include "Model/Representations/RGPModelScene.h"
#include "RPLM.EP.Model/Model/Representations/RGPPresentationContexts.h"

namespace RPLM::CAD
{
	namespace UI
	{
		RPLMCADСonjugationCurvesCommand::RPLMCADСonjugationCurvesCommand() :
			_selectObjectControl(L"SelectObjectControl", RSCADUIW("Object")),
			_groupCurveParameters(RSCADUIW("GroupCurveParameters"), L"GroupCurveParameters"),
			_curveDegree(L"CurveDegree", RSCADUIW("CurveDegree")),
			_controlPoints(L"ControlPoints", RSCADUIW("ControlPoints"), L""),
			_knots(L"Knots", RSCADUIW("Knots"), L""),
			_buttonControlFixDerivatives(L"ButtonControlFixDerivatives", RSCADUIW("ButtonControlFixDerivatives"), L"", false, true),
			_fixBeginningCurve(L"FixBeginningCurve", RSCADUIW("FixBeginningCurve"), L"", false, true),
			_fixEndCurve(L"FixEndCurve", RSCADUIW("FixEndCurve"), L"", false, true),
			_fixOrderFirstDeriv(L"FixOrderFirstDeriv", RSCADUIW("FixOrderFirstDeriv"), false, false, false),
			_fixOrderLastDeriv(L"FixOrderLastDeriv", RSCADUIW("FixOrderLastDeriv"), false, false, false)
		{
			_dialog.SetTitle(RSCADUIW("RPLM.CAD.UI.ConjugationCurves"));

			AddOkToDialog(&_dialog);
			AddCancelToDialog(&_dialog);

			// Контрол "Выбрать объект"
			_selectObjectControl.SetPlaceholderText(RSCADUIW("SelectObject"));
			//_dialog.AddControl(_selectObjectControl);

			_filter = std::make_shared<СonjugationCurvesSelectionFilter>();
			_selected = std::make_shared<EP::Model::SelectionContainer>(GetDocument().get());

			//// Степень кривой
			//_groupCurveParameters.AddControl(_curveDegree);

			//// Контрольные точки
			//_groupCurveParameters.AddControl(_controlPoints);
			//_controlPoints.SelectFile();

			//// Узловой вектор
			//_groupCurveParameters.AddControl(_knots);
			//_knots.SelectFile();

			// Степень кривой
			_dialog.AddControl(_curveDegree);

			// Контрольные точки
			_dialog.AddControl(_controlPoints);
			_controlPoints.SelectFile();

			// Узловой вектор
			_dialog.AddControl(_knots);
			_knots.SelectFile();

			_dialog.AddControl(_fixBeginningCurve);
			_dialog.AddControl(_fixEndCurve);

			//_dialog.AddControl(_groupCurveParameters);

			// Чекбокс фиксации производных
			//_dialog.AddControl(_buttonControlFixDerivatives);

			// Порядок первой производной
			_groupFixOrderDerivs.AddControl(_fixOrderFirstDeriv);
			// Порядок последней производной
			_groupFixOrderDerivs.AddControl(_fixOrderLastDeriv);

			_groupFixOrderDerivs.SetHidden(true);
			_dialog.AddControl(_groupFixOrderDerivs);

			_ok.PressEvent = std::bind(&RPLMCADСonjugationCurvesCommand::onOK, this, std::placeholders::_1);
			_dialog.OnCloseEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnCloseDialog, this);
			_selectObjectControl.ClearObjectEvent = std::bind(&RPLMCADСonjugationCurvesCommand::onClearSelectObjectControl, this, std::placeholders::_1);
			//_selectObjectControl.FocusSetEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnFocusSelectObjectControl, this, std::placeholders::_1);
			//_buttonControlFixDerivatives.PressEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnFixateDerivates, this, std::placeholders::_1);

			/// Получить путь к виду, на котором выполняется команда
			///<returns>Путь (иерархия) видов, на котором выполняется команда</returns>
			//RPLM::EP::Model::ObjectVector GetPathToLayout() { return _pathToLayout; }
			
			//_singleObjectElement.ClearObjectEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnDischargeSingleObjectElement, this, std::placeholders::_1);
			//_singleObjectElement.FocusSetEvent = std::bind(&RPLMCADСonjugationCurvesCommand::OnFocusObjectElement, this, std::placeholders::_1);
			//_layoutControl.LayoutSelectedEvent = std::bind(&AddDimensionCommand::OnLayoutSelected, this, std::placeholders::_1);
		}

		RPLMCADСonjugationCurvesCommand::~RPLMCADСonjugationCurvesCommand()
		{
		}

		bool RPLMCADСonjugationCurvesCommand::Start(EP::UI::StartCommandParameters& iParameters)
		{
			if (!Command::Start(iParameters))
			{
				return false;
			}

			CreateCommandDialog(_dialog, GetMainWindow(), GetDocument());
			_dialog.NeedToAdjust();
			_dialog.Show();

			return true;
		}

		void RPLMCADСonjugationCurvesCommand::Finish()
		{
			_dialog.Destroy();
			SetPrompt(GetView(), _STR(""));
			EmptyDynamicGraphics(GetView());
			Command::Finish();
			EP::Model::Session::GetSession()->GetExternal().DeleteUnusedData();
		}

		RPLM::EP::Model::ObjectSelectionPtr RPLMCADСonjugationCurvesCommand::SelectObject(RPLM::EP::UI::SelectObjectParameters& iParameters)
		{
			auto selection = iParameters.Selection();

			if (auto objModel = std::dynamic_pointer_cast<RPLM::EP::Model::Object>(selection->GetObject()))
			{
				_selectObjectControl.SetObject(objModel);
				_selectedCurve = objModel;
			}

			return selection;
		}

		RPLM::EP::Model::SelectionFilterPtr RPLMCADСonjugationCurvesCommand::GetFilter() const
		{
			return _filter;
		}

		void RPLMCADСonjugationCurvesCommand::onOK(EP::UI::ButtonControl& iControl)
		{
			//bool typeSpline;
			//auto curve = std::static_pointer_cast<EP::Model::Curve2D>(_selectedCurve);
			//curve->GetCurveData().GetSplineInterpolationType(typeSpline);

			/*if (typeSpline && curve->GetDependencies())
			{
				EP::DoubleArray knots;

				for (auto dep : curve->GetDependencies().Get())
				{
					auto interpolationDep = std::static_pointer_cast<RPLM::EP::Model::Curve>(dep);

					if (interpolationDep == nullptr)
						continue;

					for (const auto knot : interpolationDep->GetKnots())
					{
						knots.push_back(knot->GetValue());
					}

					RGK::NURBSCurve origiganalCurve;
					RGK::Vector<RGK::Math::Vector3D> controlPoints{};
					RGK::NURBSCurve::Create(rgkContext, controlPoints, 3, {}, false, origiganalCurve);
				}
			}*/

			{
				// Получаем введённые значения из контролов
				Base::Framework::String controlPointsPath = _controlPoints.GetFullName();
				Base::Framework::String knotsPath = _knots.GetFullName();
				int degree = _curveDegree.GetIntValue();

				if (controlPointsPath.empty() || knotsPath.empty() || degree <= 0)
				{
					Terminate();
					return;
				}

				// Считываем данные из файлов
				RGK::Vector<RGK::Math::Vector3D> controlPoints = CAD::Utils::readControlPointsFromFile(controlPointsPath);
				Math::Geometry2D::Geometry::DoubleArray knots = CAD::Utils::readKnotsFromFile(knotsPath);

				RGK::Context rgkContext;
				EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

				// Создаём объект исходной кривой
				RGK::NURBSCurve origiganalCurve;
				bool isPeriodic = false;
				RGK::NURBSCurve::Create(rgkContext, controlPoints, degree, knots, isPeriodic, origiganalCurve);
				
				auto conjugetionMethod = std::make_unique<CAD::ConjugationCurves::ConjugationMethod>();

				// Выполнение сопряжения исходной кривой с фиксацией производных
				RGK::NURBSCurve conjugatedCurve = conjugetionMethod->conjugateCurve(origiganalCurve, _fixBeginningCurve.IsChecked(), _fixEndCurve.IsChecked());

				// Записываем контрольные точки новой кривой в файл
				//CAD::Utils::saveControlPointsInFile(_STRING("C:\\Work\\rplm.all\\src\\SampleRPLM\\TempFile.txt"), conjugatedCurve.GetControlPoints());

				drawCurve(conjugatedCurve);
			}

			Terminate();
		}

		void RPLMCADСonjugationCurvesCommand::onClearSelectObjectControl(EP::UI::SingleObjectControl& iControl)
		{
			_selectObjectControl.Clear();
			EP::Model::MarkContainer* container = GetMarkContainer();
			container->RemoveAll(true);
		}

		//void RPLMCADСonjugationCurvesCommand::OnFocusSelectObjectControl(EP::UI::SingleObjectControl& iControl)
		//{
		//	_selectObjectControl.SetActive(true);
		//}

		//void RPLMCADСonjugationCurvesCommand::OnFixateDerivates(EP::UI::ButtonControl& iControl)
		//{
		//	_groupFixOrderDerivs.SetHidden(!_groupFixOrderDerivs.IsHidden());
		//}

		void RPLMCADСonjugationCurvesCommand::drawCurve(const RGK::NURBSCurve& iNurbs) const
		{
			RGK::Context context;
			RPLM::EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(context);

			RGK::BodyConstructor::Data data(0, RGK::Body::Type::Wire);
			data.CreateCoEdgeParametricCurvesAutomatically(true);

			RGK::Math::Vector3DArray points = iNurbs.GetControlPoints();
			std::vector<double> weights = iNurbs.GetWeights();
			int degree = iNurbs.GetDegree();
			std::vector<double> knots = iNurbs.GetKnots();
			bool isPeriodic = false;

			auto tolerance = context.GetLinearPrecision();
			data.AddVertex(points.front(), tolerance);
			data.AddVertex(points.back(), tolerance);

			int ends[2] = { 0, 1 };
			double interval[2] = { knots.front(), knots.back() };
			data.AddEdge(ends, iNurbs, true, interval, tolerance);

			RGK::BodyConstructor::Report report;
			if (RGK::BodyConstructor::Create(context, data, report) != RGK::Success)
				return;

			RPLM::EP::Model::ModelScenePtr modelScene = nullptr;

			for (auto i = 0; i < GetDocument()->Representations().Size(); ++i)
			{
				auto presentation = GetDocument()->Representations()[i];
				if (!presentation)
					continue;

				if (presentation->IsTypeOf(RPLM::EP::Model::ModelScene::ClassID()))
				{
					modelScene = std::dynamic_pointer_cast<RPLM::EP::Model::ModelScene>(presentation);
					break;
				}
			}

			RPLM::EP::Model::EditDocument edit(GetDocument(), _STR("BodyCreating"));
			{
				auto bodyObject = std::make_shared<RPLM::EP::Model::BodyObject>((report.GetBody()));
				GetDocument()->Objects().AddObject(bodyObject);
				modelScene->EditReferences()->AddObject(bodyObject);

				RGK::Context rgkContext;
				RPLM::EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);
				RPLM::EP::Model::Regeneration::RegenerationContext regenerationContext(GetDocument(), &rgkContext);
				modelScene->Update(RPLM::EP::Model::PresentationUpdateContext(&regenerationContext));
			}

			edit.End(false);
		}

		/*void RPLMCADСonjugationCurvesCommand::OnDischargeSingleObjectElement(EP::UI::SingleObjectControl& iControl)
		{
			CheckOKButton();

			EP::Model::MarkContainer* container = GetMarkContainer();
			container->RemoveAll(true);
		}*/

		/*void RPLMCADСonjugationCurvesCommand::OnFocusObjectElement(EP::UI::SingleObjectControl& iControl)
		{
			_selectObjectControl.SetActive(false);
		}*/

		bool RPLMCADСonjugationCurvesCommand::OnCloseDialog()
		{
			Terminate();
			return false;
		}

		RPLM::EP::Model::ObjectSelectionPtr СonjugationCurvesSelectionFilter::Select(const RPLM::EP::Model::ObjectSelectionPtr& iSelectionObject) const
		{
			RPLM::EP::Model::ObjectPtr obj = iSelectionObject->GetObject();
			RPLM::EP::Model::ObjectPtr objModel = std::dynamic_pointer_cast<RPLM::EP::Model::Object>(obj);

			return objModel != nullptr ? iSelectionObject : nullptr;
		}
	}
}