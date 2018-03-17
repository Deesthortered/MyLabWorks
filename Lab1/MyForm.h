#pragma once

namespace Lab1 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// —водка дл€ MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: OpenTK::GLControl^  GLFrame;
	protected:

	private: System::Windows::Forms::ComboBox^  ShapeListBox;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  EdgeLenghtBox;

	protected:

	protected:

	private:
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->GLFrame = (gcnew OpenTK::GLControl());
			this->ShapeListBox = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->EdgeLenghtBox = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// GLFrame
			// 
			this->GLFrame->BackColor = System::Drawing::Color::Black;
			this->GLFrame->Location = System::Drawing::Point(13, 61);
			this->GLFrame->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->GLFrame->Name = L"GLFrame";
			this->GLFrame->Size = System::Drawing::Size(774, 614);
			this->GLFrame->TabIndex = 0;
			this->GLFrame->VSync = false;
			// 
			// ShapeListBox
			// 
			this->ShapeListBox->FormattingEnabled = true;
			this->ShapeListBox->Location = System::Drawing::Point(112, 6);
			this->ShapeListBox->Name = L"ShapeListBox";
			this->ShapeListBox->Size = System::Drawing::Size(205, 24);
			this->ShapeListBox->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(94, 17);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Select shape:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(342, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(141, 17);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Enter lenght of edge:";
			// 
			// EdgeLenghtBox
			// 
			this->EdgeLenghtBox->Location = System::Drawing::Point(489, 8);
			this->EdgeLenghtBox->Name = L"EdgeLenghtBox";
			this->EdgeLenghtBox->Size = System::Drawing::Size(168, 22);
			this->EdgeLenghtBox->TabIndex = 4;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(800, 688);
			this->Controls->Add(this->EdgeLenghtBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->ShapeListBox);
			this->Controls->Add(this->GLFrame);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->Text = L"Lab1. 3D object is OpenGL.";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
