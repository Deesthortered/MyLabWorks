#pragma once

namespace Lab1 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace OpenTK::Platform::Windows;
	using namespace OpenTK::Graphics::OpenGL;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		enum class Shapes
		{
			Cube, Pyramid
		};
		ref class Engine
		{
			Shapes current_shape;
			size_t edge_lenght;

			int height;
			int width;
			int depht;
		public:
			Engine()
			{
				this->current_shape = Shapes::Cube;
				this->edge_lenght = 1;

				this->height = 0;
				this->width = 0;
				this->depht = 0;
			}
			~Engine() {};

			void SetShape(Shapes s)
			{
				this->current_shape = s;
			}
			void SetLenght(size_t l)
			{
				this->edge_lenght = l;
			}

			void InitializeOpenGL(int h, int w)
			{
				this->height = h;
				this->width = w;
				this->depht = w;

				GL::ClearColor(0.0, 0.0, 0.0, 1.0);
				GL::MatrixMode(MatrixMode::Projection);
				GL::LoadIdentity();
				GL::Ortho(-this->width >> 1, this->width >> 1, -this->height >> 1, this->height >> 1, -this->depht >> 1, this->depht >> 1);
				GL::MatrixMode(MatrixMode::Modelview);
				GL::Viewport(0, 0, this->width, this->height);
			}

		};

	public:	MyForm(void)
		{
			InitializeComponent();
		}
	protected: ~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: OpenTK::GLControl^  GLFrame;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  EdgeLenghtBox;
	public: System::Windows::Forms::ComboBox^  ShapeListBox;
	private: System::ComponentModel::Container ^components;
	private: Engine engine;
#pragma region Windows Form Designer generated code
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
			this->GLFrame->Location = System::Drawing::Point(13, 43);
			this->GLFrame->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->GLFrame->Name = L"GLFrame";
			this->GLFrame->Size = System::Drawing::Size(774, 632);
			this->GLFrame->TabIndex = 0;
			this->GLFrame->VSync = false;
			// 
			// ShapeListBox
			// 
			this->ShapeListBox->FormattingEnabled = true;
			this->ShapeListBox->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Cube", L"Pyramid (3 vertices)" });
			this->ShapeListBox->Location = System::Drawing::Point(112, 12);
			this->ShapeListBox->Name = L"ShapeListBox";
			this->ShapeListBox->Size = System::Drawing::Size(161, 24);
			this->ShapeListBox->Sorted = true;
			this->ShapeListBox->TabIndex = 1;
			this->ShapeListBox->Text = L"Cube";
			this->ShapeListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::ShapeListBox_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 15);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(94, 17);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Select shape:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(289, 15);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(141, 17);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Enter lenght of edge:";
			// 
			// EdgeLenghtBox
			// 
			this->EdgeLenghtBox->Location = System::Drawing::Point(436, 12);
			this->EdgeLenghtBox->Name = L"EdgeLenghtBox";
			this->EdgeLenghtBox->Size = System::Drawing::Size(168, 22);
			this->EdgeLenghtBox->TabIndex = 4;
			this->EdgeLenghtBox->Text = L"1";
			this->EdgeLenghtBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MyForm::EdgeLenghtBox_KeyDown);
			this->EdgeLenghtBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::EdgeLenghtBox_KeyPress);
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
			//
			// Engine
			//
			this->engine.InitializeOpenGL(this->GLFrame->Height, this->GLFrame->Width);
		}
#pragma endregion
	// Events

	private: System::Void ShapeListBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
	{
		if (ShapeListBox->SelectedItem == ShapeListBox->Items[0]) 
			engine.SetShape(Shapes::Cube);
		else if (ShapeListBox->SelectedItem == ShapeListBox->Items[1])
			engine.SetShape(Shapes::Pyramid);
	}
	private: System::Void EdgeLenghtBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
			e->Handled = true;
	}
	private: System::Void EdgeLenghtBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
	{
		if (e->KeyCode != Keys::Enter) return;
		engine.SetLenght(Convert::ToInt32(EdgeLenghtBox->Text));
	}
};
}

// MessageBox::Show(selectedState, "Та не, наебал))", MessageBoxButtons::OK, MessageBoxIcon::Error);
// String^ selectedState = ShapeListBox->SelectedItem->ToString();