﻿#pragma once

#include "RGPCommand.h"

namespace RPLM::CAD
{
	namespace UI
	{
		/// <summary>Класс команды для создания сопряжения кривых</summary>
		class RPLMCADСonjugationCurvesCommand: public EP::UI::Command
		{
		public:
			/// <summary>Конструктор для создания команды сопряжения кривых</summary>
		 	RPLMCADСonjugationCurvesCommand();

			/// <summary>Деструктор</summary>
			~RPLMCADСonjugationCurvesCommand();

			/// <summary>Запуск команды</summary>
			/// <param name="iParameters">Параметры запуска</param>
			virtual bool Start(EP::UI::StartCommandParameters& iParameters) override;

			/// <summary>Завершение команды</summary>
			virtual void Finish() override;

			/// <summary>Отработка выбора объекта</summary>
			/// <param name="iParameters">Информация о выбранных объектах</param>
			/// <returns>Выбранный объект</param>
			EP::Model::ObjectSelectionPtr SelectObject(EP::UI::SelectObjectParameters& iParameters) override;

			/// <summary>Идентификатор команды</summary>
			virtual std::string GetID() override { return "RPLM.CAD.ConjugationCurves"; }

			/// <summary>Получить фильтр команды</summary>
			EP::Model::SelectionFilterPtr GetFilter() const override;

		private:
			/// Обработчик кнопки Ok
			void onOK(EP::UI::ButtonControl& iControl);
			
			/// Обработчик очистки контрола выбора объекта
			void onClearSelectObjectControl(EP::UI::SingleObjectControl& iControl);

			// void OnFocusSelectObjectControl(EP::UI::SingleObjectControl& iControl);

			// Скрывает/показывает группу элементов фиксации порядка производных
			// void OnFixateDerivates(EP::UI::ButtonControl& iControl);

			void drawCurve(const RGK::NURBSCurve& iNurbs) const;

			// void OnDischargeSingleObjectElement(EP::UI::SingleObjectControl& iControl);
			// void OnFocusObjectElement(EP::UI::SingleObjectControl& iControl);			
			
			/// Событие о закрытии диалога
			bool OnCloseDialog();

			/// Получение диалога команды
			EP::UI::ControlLayout* GetDialog() override { return &_dialog; }

			EP::Model::ObjectPtr _selectedCurve;

			/// Диалоговое окно
			EP::UI::ControlLayout _dialog;

			/// Контрол "Выбрать объект"
			EP::UI::SingleObjectControl _selectObjectControl;

			/// Группа параметров кривой
			EP::UI::ControlGroup _groupCurveParameters;

			/// Степень кривой
			EP::UI::EditControl _curveDegree;

			/// Котролы для чтения из файла
			EP::UI::FileNameControl _controlPoints;
			EP::UI::FileNameControl _knots;

			// Контрол фиксации начала и конца кривой
			EP::UI::ButtonControl _fixBeginningCurve;
			EP::UI::ButtonControl _fixEndCurve;

			/// Кнопка и группа для фиксации производных
			EP::UI::ButtonControl _buttonControlFixDerivatives;
			EP::UI::ControlGroup _groupFixOrderDerivs;
			EP::UI::EditControl _fixOrderFirstDeriv;
			EP::UI::EditControl _fixOrderLastDeriv;
			
			EP::Model::SelectionFilterPtr _filter;
			std::shared_ptr<EP::Model::SelectionContainer> _selected;

			/// Путь к странице
			EP::Model::ObjectVector _pathToLayout;
		};		

		class СonjugationCurvesSelectionFilter final : public EP::Model::SelectionFilter
		{
		public:
			СonjugationCurvesSelectionFilter() {}
			virtual ~СonjugationCurvesSelectionFilter() {}

			bool Is3DSelect() const override
			{
				return true;
			}

			virtual EP::Model::ObjectSelectionPtr Select(const EP::Model::ObjectSelectionPtr& iSelectionObject) const override;
		};
	}
}