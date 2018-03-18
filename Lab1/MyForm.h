#pragma once

namespace Lab1 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Media;
	using namespace OpenTK::Platform::Windows;
	using namespace OpenTK::Graphics::OpenGL;

	const int FrameHeight = 500;
	const int FrameWidth  = 500;
	const int FrameDepth  = 500;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		enum class ShapeType
		{
			Cube, Pyramid
		};
		ref class Shape abstract
		{
		protected:
			int x, y, z;
			int xy, xz, yz;
			size_t l;
		public:
			virtual void SetData(int x, int y, int z, int xy, int xz, int yz, size_t l)
			{
				this->x = x;
				this->y = y;
				this->z = z;
				this->xy = xy;
				this->xz = xz;
				this->yz = yz;
				this->l = l;
			}
			virtual void Draw() abstract;
		};
		ref class Cube : public Shape
		{
			int x1, x2, x3, x4, x5, x6, x7, x8;
			int y1, y2, y3, y4, y5, y6, y7, y8;
			int z1, z2, z3, z4, z5, z6, z7, z8;
		public:
			virtual void SetData(int x, int y, int z, int xy, int xz, int yz, size_t l) override
			{
				Shape::SetData(x, y, z, xy, xz, yz, l);
				x1 = x2 = x5 = x6 = (int)(x - l * Math::Cos((xz*Math::PI) / 180));
				x3 = x4 = x7 = x8 = (int)(x + l * Math::Cos((xz*Math::PI) / 180));
				y1 = y2 = y3 = y4 = (int)(y - l * Math::Cos((xy*Math::PI) / 180));
				y5 = y6 = y7 = y8 = (int)(y + l * Math::Cos((xy*Math::PI) / 180));
				z1 = z4 = z5 = z8 = (int)(z - l * Math::Cos((yz*Math::PI) / 180));
				z2 = z3 = z6 = z7 = (int)(z + l * Math::Cos((yz*Math::PI) / 180));
			}
			virtual void Draw() override
			{
				GL::Begin(BeginMode::Quads);
				GL::Color3(1.0, 1.0, 1.0);
				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x2, y2, z2);
				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x4, y4, z4);

				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x8, y8, z8);
				GL::Vertex3(x7, y7, z7);

				GL::Vertex3(x8, y8, z8);
				GL::Vertex3(x7, y7, z7);
				GL::Vertex3(x6, y6, z6);
				GL::Vertex3(x5, y5, z5);

				GL::Vertex3(x6, y6, z6);
				GL::Vertex3(x5, y5, z5);
				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x2, y2, z2);

				GL::Vertex3(x2, y2, z2);
				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x7, y7, z7);
				GL::Vertex3(x6, y6, z6);

				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x5, y5, z5);
				GL::Vertex3(x8, y8, z8);
				GL::End();
			}
		};
		ref class Pyramid : public Shape
		{
			int x1, x2, x3, x4;
			int y1, y2, y3, y4;
			int z1, z2, z3, z4;
		public:
			virtual void SetData(int x, int y, int z, int xy, int xz, int zy, size_t l) override
			{
				Shape::SetData(x, y, z, xy, xz, zy, l);

			}
			virtual void Draw() override
			{
				GL::Begin(BeginMode::Triangles);
				GL::Color3(1.0, 1.0, 1.0);

				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x2, y2, z2);
				GL::Vertex3(x3, y3, z3);

				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x3, y3, z3);

				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x2, y2, z2);

				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x2, y2, z2);
				GL::End();
			}
		};

		ref class Engine
		{
			List<Shape^> shape_list;
		public:
			void CreateShape(ShapeType type, size_t l, int x, int y, int z, int xy, int xz, int yz)
			{
				Shape^ s;
				if (type == ShapeType::Cube) s = gcnew Cube();
				else s = gcnew Pyramid();
				s->SetData(x, y, z, xy, xz, yz, l);
				this->shape_list.Add(s);
			}
			void DrawShapes()
			{
				for each (Shape^ cur in shape_list)
				{
					cur->Draw();
				}
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
	private: System::Windows::Forms::ComboBox^  ShapeListBox;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  tb_X;
	private: System::Windows::Forms::TextBox^  tb_Y;
	private: System::Windows::Forms::TextBox^  tb_Z;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::TextBox^  tb_XY;
	private: System::Windows::Forms::TextBox^  tb_XZ;
	private: System::Windows::Forms::TextBox^  tb_YZ;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Button^  but_AddShape;
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
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->tb_X = (gcnew System::Windows::Forms::TextBox());
			this->tb_Y = (gcnew System::Windows::Forms::TextBox());
			this->tb_Z = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->tb_XY = (gcnew System::Windows::Forms::TextBox());
			this->tb_XZ = (gcnew System::Windows::Forms::TextBox());
			this->tb_YZ = (gcnew System::Windows::Forms::TextBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->but_AddShape = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// GLFrame
			// 
			this->GLFrame->BackColor = System::Drawing::Color::Black;
			this->GLFrame->Location = System::Drawing::Point(7, 112);
			this->GLFrame->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->GLFrame->Name = L"GLFrame";
			this->GLFrame->Size = System::Drawing::Size(826, 627);
			this->GLFrame->TabIndex = 0;
			this->GLFrame->VSync = false;
			this->GLFrame->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::GLFrame_KeyPress);
			// 
			// ShapeListBox
			// 
			this->ShapeListBox->FormattingEnabled = true;
			this->ShapeListBox->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Cube", L"Pyramid" });
			this->ShapeListBox->Location = System::Drawing::Point(94, 30);
			this->ShapeListBox->Name = L"ShapeListBox";
			this->ShapeListBox->Size = System::Drawing::Size(108, 24);
			this->ShapeListBox->TabIndex = 1;
			this->ShapeListBox->Text = L"Cube";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(39, 33);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 17);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Shape:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(4, 63);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(88, 17);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Edge lenght:";
			// 
			// EdgeLenghtBox
			// 
			this->EdgeLenghtBox->Location = System::Drawing::Point(94, 60);
			this->EdgeLenghtBox->Name = L"EdgeLenghtBox";
			this->EdgeLenghtBox->Size = System::Drawing::Size(108, 22);
			this->EdgeLenghtBox->TabIndex = 4;
			this->EdgeLenghtBox->Text = L"1";
			this->EdgeLenghtBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::EdgeLenghtBox_KeyPress);
			this->EdgeLenghtBox->Leave += gcnew System::EventHandler(this, &MyForm::EdgeLenghtBox_Leave);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(236, 30);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(29, 17);
			this->label3->TabIndex = 5;
			this->label3->Text = L"X =";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(236, 58);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(33, 17);
			this->label4->TabIndex = 6;
			this->label4->Text = L"Y = ";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(236, 83);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(33, 17);
			this->label5->TabIndex = 7;
			this->label5->Text = L"Z = ";
			// 
			// tb_X
			// 
			this->tb_X->Location = System::Drawing::Point(263, 27);
			this->tb_X->Name = L"tb_X";
			this->tb_X->Size = System::Drawing::Size(46, 22);
			this->tb_X->TabIndex = 8;
			this->tb_X->Text = L"0";
			this->tb_X->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_X_KeyPress);
			this->tb_X->Leave += gcnew System::EventHandler(this, &MyForm::tb_X_Leave);
			// 
			// tb_Y
			// 
			this->tb_Y->Location = System::Drawing::Point(263, 55);
			this->tb_Y->Name = L"tb_Y";
			this->tb_Y->Size = System::Drawing::Size(46, 22);
			this->tb_Y->TabIndex = 9;
			this->tb_Y->Text = L"0";
			this->tb_Y->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_Y_KeyPress);
			this->tb_Y->Leave += gcnew System::EventHandler(this, &MyForm::tb_Y_Leave);
			// 
			// tb_Z
			// 
			this->tb_Z->Location = System::Drawing::Point(263, 83);
			this->tb_Z->Name = L"tb_Z";
			this->tb_Z->Size = System::Drawing::Size(46, 22);
			this->tb_Z->TabIndex = 10;
			this->tb_Z->Text = L"0";
			this->tb_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_Z_KeyPress);
			this->tb_Z->Leave += gcnew System::EventHandler(this, &MyForm::tb_Z_Leave);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label6->Location = System::Drawing::Point(221, 5);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(88, 17);
			this->label6->TabIndex = 11;
			this->label6->Text = L"Coordinates:";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(347, 28);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(38, 17);
			this->label7->TabIndex = 12;
			this->label7->Text = L"XY =";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(347, 58);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(38, 17);
			this->label8->TabIndex = 13;
			this->label8->Text = L"XZ =";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(347, 86);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(38, 17);
			this->label9->TabIndex = 14;
			this->label9->Text = L"YZ =";
			// 
			// tb_XY
			// 
			this->tb_XY->Location = System::Drawing::Point(391, 25);
			this->tb_XY->Name = L"tb_XY";
			this->tb_XY->Size = System::Drawing::Size(46, 22);
			this->tb_XY->TabIndex = 15;
			this->tb_XY->Text = L"0";
			this->tb_XY->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_XY_KeyPress);
			this->tb_XY->Leave += gcnew System::EventHandler(this, &MyForm::tb_XY_Leave);
			// 
			// tb_XZ
			// 
			this->tb_XZ->Location = System::Drawing::Point(391, 55);
			this->tb_XZ->Name = L"tb_XZ";
			this->tb_XZ->Size = System::Drawing::Size(46, 22);
			this->tb_XZ->TabIndex = 16;
			this->tb_XZ->Text = L"0";
			this->tb_XZ->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_XZ_KeyPress);
			this->tb_XZ->Leave += gcnew System::EventHandler(this, &MyForm::tb_XZ_Leave);
			// 
			// tb_YZ
			// 
			this->tb_YZ->Location = System::Drawing::Point(391, 83);
			this->tb_YZ->Name = L"tb_YZ";
			this->tb_YZ->Size = System::Drawing::Size(46, 22);
			this->tb_YZ->TabIndex = 17;
			this->tb_YZ->Text = L"0";
			this->tb_YZ->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_YZ_KeyPress);
			this->tb_YZ->Leave += gcnew System::EventHandler(this, &MyForm::tb_YZ_Leave);
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label10->Location = System::Drawing::Point(382, 5);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(55, 17);
			this->label10->TabIndex = 18;
			this->label10->Text = L"Angles:";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label11->Location = System::Drawing::Point(39, 5);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(63, 17);
			this->label11->TabIndex = 19;
			this->label11->Text = L"General:";
			// 
			// but_AddShape
			// 
			this->but_AddShape->Location = System::Drawing::Point(452, 12);
			this->but_AddShape->Name = L"but_AddShape";
			this->but_AddShape->Size = System::Drawing::Size(67, 93);
			this->but_AddShape->TabIndex = 20;
			this->but_AddShape->Text = L"Add Shape";
			this->but_AddShape->UseVisualStyleBackColor = true;
			this->but_AddShape->Click += gcnew System::EventHandler(this, &MyForm::but_AddShape_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(846, 747);
			this->Controls->Add(this->but_AddShape);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->tb_YZ);
			this->Controls->Add(this->tb_XZ);
			this->Controls->Add(this->tb_XY);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->tb_Z);
			this->Controls->Add(this->tb_Y);
			this->Controls->Add(this->tb_X);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->EdgeLenghtBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->ShapeListBox);
			this->Controls->Add(this->GLFrame);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->Text = L"Lab1. 3D object is OpenGL.";
			this->Shown += gcnew System::EventHandler(this, &MyForm::MyForm_Shown);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	// Functions and Events
	private: void InitializeGL()
	{
		GL::ClearColor(Color::Black);
		GL::MatrixMode(MatrixMode::Projection);
		GL::LoadIdentity();
		GL::Ortho(-(FrameWidth >> 1), FrameWidth >> 1, -(FrameHeight >> 1), FrameHeight >> 1, -(FrameDepth >> 1), FrameDepth >> 1);
		GL::Viewport(0, 0, FrameWidth, FrameHeight);
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
		GL::MatrixMode(MatrixMode::Modelview);
		GL::LoadIdentity();
	}
	private: void DrawOrthLines()
	{
		GL::Begin(BeginMode::Lines);

		GL::Color3(1.0, 0.0, 0.0);
		GL::Vertex3(0.0, 0.0, 0.0);
		GL::Vertex3(double(FrameWidth >> 1), 0.0, 0.0);

		GL::Color3(0.0, 1.0, 0.0);
		GL::Vertex3(0.0, 0.0, 0.0);
		GL::Vertex3(0.0, double(FrameHeight >> 1), 0.0);

		GL::Color3(0.0, 0.0, 1.0);
		GL::Vertex3(0.0, 0.0, 0.0);
		GL::Vertex3(0.0, 0.0, double(FrameDepth >> 1));

		GL::End();
	}

	private: System::Void MyForm_Shown(System::Object^  sender, System::EventArgs^  e)
	{
		InitializeGL();
		DrawOrthLines();
		GLFrame->SwapBuffers();
	}
	private: System::Void EdgeLenghtBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
			e->Handled = true;
	}
	private: System::Void EdgeLenghtBox_Leave(System::Object^  sender, System::EventArgs^  e)
	{
		if (EdgeLenghtBox->Text->Length == 0) EdgeLenghtBox->Text = "1";
	}
	private: System::Void tb_X_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb_X->Text->Length == 0 || tb_X->Text == "-") tb_X->Text = "0";
	}
	private: System::Void tb_Y_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb_Y->Text->Length == 0 || tb_Y->Text == "-") tb_Y->Text = "0";
	}
	private: System::Void tb_Z_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb_Z->Text->Length == 0 || tb_X->Text == "Z") tb_Z->Text = "0";
	}
	private: System::Void tb_XY_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb_XY->Text->Length == 0 || tb_XY->Text == "-") tb_XY->Text = "0";
	}
	private: System::Void tb_XZ_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb_XZ->Text->Length == 0 || tb_XZ->Text == "-") tb_XZ->Text = "0";
	}
	private: System::Void tb_YZ_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb_YZ->Text->Length == 0 || tb_YZ->Text == "-") tb_YZ->Text = "0";
	}
	private: System::Void tb_X_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb_X->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb_Y_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb_Y->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb_Z_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb_Z->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb_XY_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb_XY->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb_XZ_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb_XZ->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb_YZ_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb_YZ->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}

	private: System::Void but_AddShape_Click(System::Object^  sender, System::EventArgs^  e)
	{
		SystemSounds::Beep->Play();
		ShapeType type;
		if (ShapeListBox->SelectedItem == ShapeListBox->Items[0]) type = ShapeType::Cube;
		else if (ShapeListBox->SelectedItem == ShapeListBox->Items[1]) type = ShapeType::Pyramid;
		engine.CreateShape(type, Convert::ToInt32(EdgeLenghtBox->Text),
			Convert::ToInt32(tb_X->Text),
			Convert::ToInt32(tb_Y->Text),
			Convert::ToInt32(tb_Z->Text),
			Convert::ToInt32(tb_XY->Text),
			Convert::ToInt32(tb_XZ->Text),
			Convert::ToInt32(tb_YZ->Text));
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
		DrawOrthLines();
		engine.DrawShapes();
		GLFrame->SwapBuffers();
	}
	private: System::Void GLFrame_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
		switch (e->KeyChar)
		{
		case 'a': { GL::Translate( 1.0,  0.0,  0.0); } break;
		case 'd': { GL::Translate(-1.0,  0.0,  0.0); } break;
		case 'w': { GL::Translate( 0.0,  1.0,  0.0); } break;
		case 's': { GL::Translate( 0.0, -1.0,  0.0); } break;
		case 'z': { GL::Translate( 0.0,  0.0,  1.0); } break;
		case 'c': { GL::Translate( 0.0,  0.0, -1.0); } break;

		case 'q': { GL::Scale(0.9, 0.9, 0.9); } break;
		case 'e': { GL::Scale(1.1, 1.1, 1.1); } break;

		case 'r': { GL::Rotate( 1, 0.0, 0.0, 1.0); } break;
		case 'f': { GL::Rotate(-1, 0.0, 0.0, 1.0); } break;
		case 't': { GL::Rotate( 1, 0.0, 1.0, 0.0); } break;
		case 'g': { GL::Rotate(-1, 0.0, 1.0, 0.0); } break;
		case 'y': { GL::Rotate( 1, 1.0, 0.0, 0.0); } break;
		case 'h': { GL::Rotate(-1, 1.0, 0.0, 0.0); } break;
		}
		DrawOrthLines();
		engine.DrawShapes();
		GLFrame->SwapBuffers();
	}
};
}