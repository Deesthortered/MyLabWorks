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

	int FrameHeight;
	int FrameWidth;
	int FrameDepth;

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
			double x_coor, y_coor, z_coor;
			double x_rot, y_rot, z_rot;
			double angle;
			size_t l;
			Color  color;
		public:
			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, double angle)
			{
				this->color = color;
				this->l = l;
				this->x_coor = x_coor;
				this->y_coor = y_coor;
				this->z_coor = z_coor;
				this->x_rot = x_rot;
				this->y_rot = y_rot;
				this->z_rot = z_rot;
				this->angle = angle;
			}
			virtual void Draw() abstract;
		};
		ref class Cube : public Shape
		{
			double x1, x2, x3, x4, x5, x6, x7, x8;
			double y1, y2, y3, y4, y5, y6, y7, y8;
			double z1, z2, z3, z4, z5, z6, z7, z8;
		public:
			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, double angle) override
			{
				Shape::SetData(color, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, angle);
				x1 = x2 = x5 = x6 = x_coor - l / 2;
				x3 = x4 = x7 = x8 = x_coor + l / 2;
				y1 = y2 = y3 = y4 = y_coor - l / 2;
				y5 = y6 = y7 = y8 = y_coor + l / 2;
				z1 = z4 = z5 = z8 = z_coor - l / 2;
				z2 = z3 = z6 = z7 = z_coor + l / 2;
			}
			virtual void Draw() override
			{
				GL::PushMatrix();
				GL::Rotate(this->angle, this->x_rot, this->y_rot, this->z_rot);
				GL::Begin(BeginMode::Quads);
				GL::Color3(color);
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

				GL::Color3(Color::White);
				GL::Begin(BeginMode::LineStrip);
				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x2, y2, z2);
				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x1, y1, z1);
				GL::End();
				GL::Begin(BeginMode::LineStrip);
				GL::Vertex3(x5, y5, z5);
				GL::Vertex3(x6, y6, z6);
				GL::Vertex3(x7, y7, z7);
				GL::Vertex3(x8, y8, z8);
				GL::Vertex3(x5, y5, z5);
				GL::End();
				GL::Begin(BeginMode::Lines);
				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x5, y5, z5);
				GL::Vertex3(x2, y2, z2);
				GL::Vertex3(x6, y6, z6);
				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x7, y7, z7);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x8, y8, z8);
				GL::End();
				GL::PopMatrix();
			}
		};
		ref class Pyramid : public Shape
		{
			int x1, x2, x3, x4;
			int y1, y2, y3, y4;
			int z1, z2, z3, z4;
		public:
			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, double angle) override
			{
				Shape::SetData(color, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, angle);

				x1 = x3 = x_coor - (l / Math::Sqrt(12)); x2 = x_coor + (l / Math::Sqrt(3)); x4 = x_coor;
				y1 = y2 = y3 = y_coor; y4 = y_coor + Math::Sqrt((2 * l*l) / 3);
				z1 = z_coor - (l / 2); z2= z4 = z_coor; z3 = z_coor + (l / 2);
			}
			virtual void Draw() override
			{
				GL::PushMatrix();
				GL::Rotate(this->angle, this->x_rot, this->y_rot, this->z_rot);
				GL::Begin(BeginMode::Triangles);
				GL::Color3(color);

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

				GL::Color3(Color::White);
				GL::Begin(BeginMode::LineStrip);
				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x2, y2, z2);
				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x1, y1, z1);
				GL::Vertex3(x4, y4, z4);
				GL::Vertex3(x3, y3, z3);
				GL::Vertex3(x2, y2, z2);
				GL::Vertex3(x4, y4, z4);
				GL::End();
				GL::PopMatrix();
			}
		};

		ref class Engine
		{
			List<Shape^> shape_list;
		public:
			void CreateShape(ShapeType type, Color col, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, double angle)
			{
				Shape^ s;
				if (type == ShapeType::Cube) s = gcnew Cube();
				else s = gcnew Pyramid();
				s->SetData(col, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, angle);
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
	private: System::Windows::Forms::Label^  label2Shape;
	private: System::Windows::Forms::Label^  label2EdgeLenght;
	private: System::Windows::Forms::TextBox^  tb2_leght;
	private: System::Windows::Forms::ComboBox^  ShapeListBox2;
	private: System::Windows::Forms::Label^  label2XCoord;
	private: System::Windows::Forms::Label^  label2YCoord;
	private: System::Windows::Forms::Label^  label2ZCoord;
	private: System::Windows::Forms::TextBox^  tb2_Xcoor;
	private: System::Windows::Forms::TextBox^  tb2_Ycoor;
	private: System::Windows::Forms::TextBox^  tb2_Zcoor;
	private: System::Windows::Forms::Label^  label2Coordinates;
	private: System::Windows::Forms::Label^  label2XRot;
	private: System::Windows::Forms::Label^  label2YRot;
	private: System::Windows::Forms::Label^  label2ZRot;
	private: System::Windows::Forms::TextBox^  tb2_Xrot;
	private: System::Windows::Forms::TextBox^  tb2_Yrot;
	private: System::Windows::Forms::TextBox^  tb2_Zrot;
	private: System::Windows::Forms::Label^  label2Rotation;
	private: System::Windows::Forms::Label^  lable2General;
	private: System::Windows::Forms::Button^  but2_AddShape;
	private: System::ComponentModel::Container ^components;
	private: System::Windows::Forms::TabControl^  MainTabControl;
	private: System::Windows::Forms::TabPage^  CameraTab;
	private: System::Windows::Forms::TabPage^  AddShapeTab;
	private: System::Windows::Forms::Label^  label2Color;
	private: System::Windows::Forms::ComboBox^  ColorBox2;
	private: System::Windows::Forms::Label^  label2Angle;
	private: System::Windows::Forms::TextBox^  tb2_angle;
	private: System::Windows::Forms::Label^  label1rot;
	private: System::Windows::Forms::Label^  label1scal;
	private: System::Windows::Forms::Label^  label1trans;
	private: System::Windows::Forms::TextBox^  tb1_scal;
	private: System::Windows::Forms::TextBox^  tb1_rot;
	private: System::Windows::Forms::TextBox^  tb1_trans;
	private: System::Windows::Forms::RadioButton^  rb1_free;
	private: System::Windows::Forms::RadioButton^  rb1_centr;
	private: System::Windows::Forms::TabPage^  ControlTab;

	private: Engine engine;
#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->GLFrame = (gcnew OpenTK::GLControl());
			this->ShapeListBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->label2Shape = (gcnew System::Windows::Forms::Label());
			this->label2EdgeLenght = (gcnew System::Windows::Forms::Label());
			this->tb2_leght = (gcnew System::Windows::Forms::TextBox());
			this->label2XCoord = (gcnew System::Windows::Forms::Label());
			this->label2YCoord = (gcnew System::Windows::Forms::Label());
			this->label2ZCoord = (gcnew System::Windows::Forms::Label());
			this->tb2_Xcoor = (gcnew System::Windows::Forms::TextBox());
			this->tb2_Ycoor = (gcnew System::Windows::Forms::TextBox());
			this->tb2_Zcoor = (gcnew System::Windows::Forms::TextBox());
			this->label2Coordinates = (gcnew System::Windows::Forms::Label());
			this->label2XRot = (gcnew System::Windows::Forms::Label());
			this->label2YRot = (gcnew System::Windows::Forms::Label());
			this->label2ZRot = (gcnew System::Windows::Forms::Label());
			this->tb2_Xrot = (gcnew System::Windows::Forms::TextBox());
			this->tb2_Yrot = (gcnew System::Windows::Forms::TextBox());
			this->tb2_Zrot = (gcnew System::Windows::Forms::TextBox());
			this->label2Rotation = (gcnew System::Windows::Forms::Label());
			this->lable2General = (gcnew System::Windows::Forms::Label());
			this->but2_AddShape = (gcnew System::Windows::Forms::Button());
			this->MainTabControl = (gcnew System::Windows::Forms::TabControl());
			this->CameraTab = (gcnew System::Windows::Forms::TabPage());
			this->rb1_free = (gcnew System::Windows::Forms::RadioButton());
			this->rb1_centr = (gcnew System::Windows::Forms::RadioButton());
			this->tb1_scal = (gcnew System::Windows::Forms::TextBox());
			this->tb1_rot = (gcnew System::Windows::Forms::TextBox());
			this->tb1_trans = (gcnew System::Windows::Forms::TextBox());
			this->label1rot = (gcnew System::Windows::Forms::Label());
			this->label1scal = (gcnew System::Windows::Forms::Label());
			this->label1trans = (gcnew System::Windows::Forms::Label());
			this->AddShapeTab = (gcnew System::Windows::Forms::TabPage());
			this->label2Angle = (gcnew System::Windows::Forms::Label());
			this->tb2_angle = (gcnew System::Windows::Forms::TextBox());
			this->label2Color = (gcnew System::Windows::Forms::Label());
			this->ColorBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->ControlTab = (gcnew System::Windows::Forms::TabPage());
			this->MainTabControl->SuspendLayout();
			this->CameraTab->SuspendLayout();
			this->AddShapeTab->SuspendLayout();
			this->SuspendLayout();
			// 
			// GLFrame
			// 
			this->GLFrame->BackColor = System::Drawing::Color::Black;
			this->GLFrame->Location = System::Drawing::Point(7, 171);
			this->GLFrame->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->GLFrame->Name = L"GLFrame";
			this->GLFrame->Size = System::Drawing::Size(1000, 670);
			this->GLFrame->TabIndex = 0;
			this->GLFrame->VSync = false;
			this->GLFrame->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::GLFrame_KeyPress);
			// 
			// ShapeListBox2
			// 
			this->ShapeListBox2->FormattingEnabled = true;
			this->ShapeListBox2->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Cube", L"Pyramid" });
			this->ShapeListBox2->Location = System::Drawing::Point(101, 26);
			this->ShapeListBox2->Name = L"ShapeListBox2";
			this->ShapeListBox2->Size = System::Drawing::Size(108, 24);
			this->ShapeListBox2->TabIndex = 1;
			this->ShapeListBox2->Text = L"Cube";
			// 
			// label2Shape
			// 
			this->label2Shape->AutoSize = true;
			this->label2Shape->Location = System::Drawing::Point(46, 29);
			this->label2Shape->Name = L"label2Shape";
			this->label2Shape->Size = System::Drawing::Size(53, 17);
			this->label2Shape->TabIndex = 2;
			this->label2Shape->Text = L"Shape:";
			// 
			// label2EdgeLenght
			// 
			this->label2EdgeLenght->AutoSize = true;
			this->label2EdgeLenght->Location = System::Drawing::Point(11, 89);
			this->label2EdgeLenght->Name = L"label2EdgeLenght";
			this->label2EdgeLenght->Size = System::Drawing::Size(88, 17);
			this->label2EdgeLenght->TabIndex = 3;
			this->label2EdgeLenght->Text = L"Edge lenght:";
			// 
			// tb2_leght
			// 
			this->tb2_leght->Location = System::Drawing::Point(101, 86);
			this->tb2_leght->Name = L"tb2_leght";
			this->tb2_leght->Size = System::Drawing::Size(108, 22);
			this->tb2_leght->TabIndex = 4;
			this->tb2_leght->Text = L"1";
			this->tb2_leght->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_leght_KeyPress);
			this->tb2_leght->Leave += gcnew System::EventHandler(this, &MyForm::tb2_leght_Leave);
			// 
			// label2XCoord
			// 
			this->label2XCoord->AutoSize = true;
			this->label2XCoord->Location = System::Drawing::Point(253, 31);
			this->label2XCoord->Name = L"label2XCoord";
			this->label2XCoord->Size = System::Drawing::Size(29, 17);
			this->label2XCoord->TabIndex = 5;
			this->label2XCoord->Text = L"X =";
			// 
			// label2YCoord
			// 
			this->label2YCoord->AutoSize = true;
			this->label2YCoord->Location = System::Drawing::Point(253, 59);
			this->label2YCoord->Name = L"label2YCoord";
			this->label2YCoord->Size = System::Drawing::Size(29, 17);
			this->label2YCoord->TabIndex = 6;
			this->label2YCoord->Text = L"Y =";
			// 
			// label2ZCoord
			// 
			this->label2ZCoord->AutoSize = true;
			this->label2ZCoord->Location = System::Drawing::Point(253, 87);
			this->label2ZCoord->Name = L"label2ZCoord";
			this->label2ZCoord->Size = System::Drawing::Size(29, 17);
			this->label2ZCoord->TabIndex = 7;
			this->label2ZCoord->Text = L"Z =";
			// 
			// tb2_Xcoor
			// 
			this->tb2_Xcoor->Location = System::Drawing::Point(288, 28);
			this->tb2_Xcoor->Name = L"tb2_Xcoor";
			this->tb2_Xcoor->Size = System::Drawing::Size(46, 22);
			this->tb2_Xcoor->TabIndex = 8;
			this->tb2_Xcoor->Text = L"0";
			this->tb2_Xcoor->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Xcoor_KeyPress);
			this->tb2_Xcoor->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Xcoor_Leave);
			// 
			// tb2_Ycoor
			// 
			this->tb2_Ycoor->Location = System::Drawing::Point(288, 56);
			this->tb2_Ycoor->Name = L"tb2_Ycoor";
			this->tb2_Ycoor->Size = System::Drawing::Size(46, 22);
			this->tb2_Ycoor->TabIndex = 9;
			this->tb2_Ycoor->Text = L"0";
			this->tb2_Ycoor->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Ycoor_KeyPress);
			this->tb2_Ycoor->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Ycoor_Leave);
			// 
			// tb2_Zcoor
			// 
			this->tb2_Zcoor->Location = System::Drawing::Point(288, 84);
			this->tb2_Zcoor->Name = L"tb2_Zcoor";
			this->tb2_Zcoor->Size = System::Drawing::Size(46, 22);
			this->tb2_Zcoor->TabIndex = 10;
			this->tb2_Zcoor->Text = L"0";
			this->tb2_Zcoor->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Zcoor_KeyPress);
			this->tb2_Zcoor->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Zcoor_Leave);
			// 
			// label2Coordinates
			// 
			this->label2Coordinates->AutoSize = true;
			this->label2Coordinates->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Underline,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->label2Coordinates->Location = System::Drawing::Point(246, 3);
			this->label2Coordinates->Name = L"label2Coordinates";
			this->label2Coordinates->Size = System::Drawing::Size(88, 17);
			this->label2Coordinates->TabIndex = 11;
			this->label2Coordinates->Text = L"Coordinates:";
			// 
			// label2XRot
			// 
			this->label2XRot->AutoSize = true;
			this->label2XRot->Location = System::Drawing::Point(400, 46);
			this->label2XRot->Name = L"label2XRot";
			this->label2XRot->Size = System::Drawing::Size(29, 17);
			this->label2XRot->TabIndex = 12;
			this->label2XRot->Text = L"X =";
			// 
			// label2YRot
			// 
			this->label2YRot->AutoSize = true;
			this->label2YRot->Location = System::Drawing::Point(400, 74);
			this->label2YRot->Name = L"label2YRot";
			this->label2YRot->Size = System::Drawing::Size(29, 17);
			this->label2YRot->TabIndex = 13;
			this->label2YRot->Text = L"Y =";
			// 
			// label2ZRot
			// 
			this->label2ZRot->AutoSize = true;
			this->label2ZRot->Location = System::Drawing::Point(400, 102);
			this->label2ZRot->Name = L"label2ZRot";
			this->label2ZRot->Size = System::Drawing::Size(29, 17);
			this->label2ZRot->TabIndex = 14;
			this->label2ZRot->Text = L"Z =";
			// 
			// tb2_Xrot
			// 
			this->tb2_Xrot->Location = System::Drawing::Point(435, 43);
			this->tb2_Xrot->Name = L"tb2_Xrot";
			this->tb2_Xrot->Size = System::Drawing::Size(46, 22);
			this->tb2_Xrot->TabIndex = 15;
			this->tb2_Xrot->Text = L"0";
			this->tb2_Xrot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Xrot_KeyPress);
			this->tb2_Xrot->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Xrot_Leave);
			// 
			// tb2_Yrot
			// 
			this->tb2_Yrot->Location = System::Drawing::Point(435, 71);
			this->tb2_Yrot->Name = L"tb2_Yrot";
			this->tb2_Yrot->Size = System::Drawing::Size(46, 22);
			this->tb2_Yrot->TabIndex = 16;
			this->tb2_Yrot->Text = L"0";
			this->tb2_Yrot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Yrot_KeyPress);
			this->tb2_Yrot->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Yrot_Leave);
			// 
			// tb2_Zrot
			// 
			this->tb2_Zrot->Location = System::Drawing::Point(435, 99);
			this->tb2_Zrot->Name = L"tb2_Zrot";
			this->tb2_Zrot->Size = System::Drawing::Size(46, 22);
			this->tb2_Zrot->TabIndex = 17;
			this->tb2_Zrot->Text = L"0";
			this->tb2_Zrot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Zrot_KeyPress);
			this->tb2_Zrot->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Zrot_Leave);
			// 
			// label2Rotation
			// 
			this->label2Rotation->AutoSize = true;
			this->label2Rotation->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label2Rotation->Location = System::Drawing::Point(364, 3);
			this->label2Rotation->Name = L"label2Rotation";
			this->label2Rotation->Size = System::Drawing::Size(65, 17);
			this->label2Rotation->TabIndex = 18;
			this->label2Rotation->Text = L"Rotation:";
			// 
			// lable2General
			// 
			this->lable2General->AutoSize = true;
			this->lable2General->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->lable2General->Location = System::Drawing::Point(6, 3);
			this->lable2General->Name = L"lable2General";
			this->lable2General->Size = System::Drawing::Size(63, 17);
			this->lable2General->TabIndex = 19;
			this->lable2General->Text = L"General:";
			// 
			// but2_AddShape
			// 
			this->but2_AddShape->Location = System::Drawing::Point(497, 13);
			this->but2_AddShape->Name = L"but2_AddShape";
			this->but2_AddShape->Size = System::Drawing::Size(80, 106);
			this->but2_AddShape->TabIndex = 20;
			this->but2_AddShape->Text = L"Add Shape";
			this->but2_AddShape->UseVisualStyleBackColor = true;
			this->but2_AddShape->Click += gcnew System::EventHandler(this, &MyForm::but_AddShape_Click);
			// 
			// MainTabControl
			// 
			this->MainTabControl->Controls->Add(this->CameraTab);
			this->MainTabControl->Controls->Add(this->AddShapeTab);
			this->MainTabControl->Controls->Add(this->ControlTab);
			this->MainTabControl->Location = System::Drawing::Point(12, 12);
			this->MainTabControl->Name = L"MainTabControl";
			this->MainTabControl->SelectedIndex = 0;
			this->MainTabControl->Size = System::Drawing::Size(995, 156);
			this->MainTabControl->TabIndex = 21;
			// 
			// CameraTab
			// 
			this->CameraTab->Controls->Add(this->rb1_free);
			this->CameraTab->Controls->Add(this->rb1_centr);
			this->CameraTab->Controls->Add(this->tb1_scal);
			this->CameraTab->Controls->Add(this->tb1_rot);
			this->CameraTab->Controls->Add(this->tb1_trans);
			this->CameraTab->Controls->Add(this->label1rot);
			this->CameraTab->Controls->Add(this->label1scal);
			this->CameraTab->Controls->Add(this->label1trans);
			this->CameraTab->Location = System::Drawing::Point(4, 25);
			this->CameraTab->Name = L"CameraTab";
			this->CameraTab->Padding = System::Windows::Forms::Padding(3);
			this->CameraTab->Size = System::Drawing::Size(987, 127);
			this->CameraTab->TabIndex = 0;
			this->CameraTab->Text = L"Camera";
			this->CameraTab->UseVisualStyleBackColor = true;
			// 
			// rb1_free
			// 
			this->rb1_free->AutoSize = true;
			this->rb1_free->Location = System::Drawing::Point(240, 39);
			this->rb1_free->Name = L"rb1_free";
			this->rb1_free->Size = System::Drawing::Size(109, 21);
			this->rb1_free->TabIndex = 7;
			this->rb1_free->Text = L"Free camera";
			this->rb1_free->UseVisualStyleBackColor = true;
			// 
			// rb1_centr
			// 
			this->rb1_centr->AutoSize = true;
			this->rb1_centr->Checked = true;
			this->rb1_centr->Location = System::Drawing::Point(240, 12);
			this->rb1_centr->Name = L"rb1_centr";
			this->rb1_centr->Size = System::Drawing::Size(151, 21);
			this->rb1_centr->TabIndex = 6;
			this->rb1_centr->TabStop = true;
			this->rb1_centr->Text = L"Centralized camera";
			this->rb1_centr->UseVisualStyleBackColor = true;
			// 
			// tb1_scal
			// 
			this->tb1_scal->Location = System::Drawing::Point(115, 70);
			this->tb1_scal->Name = L"tb1_scal";
			this->tb1_scal->Size = System::Drawing::Size(100, 22);
			this->tb1_scal->TabIndex = 5;
			// 
			// tb1_rot
			// 
			this->tb1_rot->Location = System::Drawing::Point(115, 41);
			this->tb1_rot->Name = L"tb1_rot";
			this->tb1_rot->Size = System::Drawing::Size(100, 22);
			this->tb1_rot->TabIndex = 4;
			// 
			// tb1_trans
			// 
			this->tb1_trans->Location = System::Drawing::Point(115, 12);
			this->tb1_trans->Name = L"tb1_trans";
			this->tb1_trans->Size = System::Drawing::Size(100, 22);
			this->tb1_trans->TabIndex = 3;
			// 
			// label1rot
			// 
			this->label1rot->AutoSize = true;
			this->label1rot->Location = System::Drawing::Point(24, 44);
			this->label1rot->Name = L"label1rot";
			this->label1rot->Size = System::Drawing::Size(85, 17);
			this->label1rot->TabIndex = 2;
			this->label1rot->Text = L"Rotate step:";
			// 
			// label1scal
			// 
			this->label1scal->AutoSize = true;
			this->label1scal->Location = System::Drawing::Point(31, 73);
			this->label1scal->Name = L"label1scal";
			this->label1scal->Size = System::Drawing::Size(78, 17);
			this->label1scal->TabIndex = 1;
			this->label1scal->Text = L"Scale step:";
			// 
			// label1trans
			// 
			this->label1trans->AutoSize = true;
			this->label1trans->Location = System::Drawing::Point(6, 12);
			this->label1trans->Name = L"label1trans";
			this->label1trans->Size = System::Drawing::Size(103, 17);
			this->label1trans->TabIndex = 0;
			this->label1trans->Text = L"Translate step:";
			// 
			// AddShapeTab
			// 
			this->AddShapeTab->Controls->Add(this->label2Angle);
			this->AddShapeTab->Controls->Add(this->tb2_angle);
			this->AddShapeTab->Controls->Add(this->label2Color);
			this->AddShapeTab->Controls->Add(this->ColorBox2);
			this->AddShapeTab->Controls->Add(this->label2EdgeLenght);
			this->AddShapeTab->Controls->Add(this->label2ZRot);
			this->AddShapeTab->Controls->Add(this->but2_AddShape);
			this->AddShapeTab->Controls->Add(this->ShapeListBox2);
			this->AddShapeTab->Controls->Add(this->lable2General);
			this->AddShapeTab->Controls->Add(this->label2Shape);
			this->AddShapeTab->Controls->Add(this->label2Rotation);
			this->AddShapeTab->Controls->Add(this->tb2_leght);
			this->AddShapeTab->Controls->Add(this->tb2_Zrot);
			this->AddShapeTab->Controls->Add(this->label2XCoord);
			this->AddShapeTab->Controls->Add(this->tb2_Yrot);
			this->AddShapeTab->Controls->Add(this->label2YCoord);
			this->AddShapeTab->Controls->Add(this->tb2_Xrot);
			this->AddShapeTab->Controls->Add(this->label2ZCoord);
			this->AddShapeTab->Controls->Add(this->tb2_Xcoor);
			this->AddShapeTab->Controls->Add(this->label2YRot);
			this->AddShapeTab->Controls->Add(this->tb2_Ycoor);
			this->AddShapeTab->Controls->Add(this->label2XRot);
			this->AddShapeTab->Controls->Add(this->tb2_Zcoor);
			this->AddShapeTab->Controls->Add(this->label2Coordinates);
			this->AddShapeTab->Location = System::Drawing::Point(4, 25);
			this->AddShapeTab->Name = L"AddShapeTab";
			this->AddShapeTab->Padding = System::Windows::Forms::Padding(3);
			this->AddShapeTab->Size = System::Drawing::Size(987, 127);
			this->AddShapeTab->TabIndex = 1;
			this->AddShapeTab->Text = L"Add shape";
			this->AddShapeTab->UseVisualStyleBackColor = true;
			// 
			// label2Angle
			// 
			this->label2Angle->AutoSize = true;
			this->label2Angle->Location = System::Drawing::Point(373, 22);
			this->label2Angle->Name = L"label2Angle";
			this->label2Angle->Size = System::Drawing::Size(56, 17);
			this->label2Angle->TabIndex = 24;
			this->label2Angle->Text = L"Angle =";
			// 
			// tb2_angle
			// 
			this->tb2_angle->Location = System::Drawing::Point(435, 17);
			this->tb2_angle->Name = L"tb2_angle";
			this->tb2_angle->Size = System::Drawing::Size(46, 22);
			this->tb2_angle->TabIndex = 23;
			this->tb2_angle->Text = L"0";
			this->tb2_angle->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_angle_KeyPress);
			this->tb2_angle->Leave += gcnew System::EventHandler(this, &MyForm::tb2_angle_Leave);
			// 
			// label2Color
			// 
			this->label2Color->AutoSize = true;
			this->label2Color->Location = System::Drawing::Point(54, 59);
			this->label2Color->Name = L"label2Color";
			this->label2Color->Size = System::Drawing::Size(45, 17);
			this->label2Color->TabIndex = 22;
			this->label2Color->Text = L"Color:";
			// 
			// ColorBox2
			// 
			this->ColorBox2->FormattingEnabled = true;
			this->ColorBox2->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
				L"Red", L"Orange", L"Yellow", L"Green", L"Aqua",
					L"Blue", L"Purple", L"Brown", L"Gray"
			});
			this->ColorBox2->Location = System::Drawing::Point(101, 56);
			this->ColorBox2->Name = L"ColorBox2";
			this->ColorBox2->Size = System::Drawing::Size(108, 24);
			this->ColorBox2->TabIndex = 21;
			this->ColorBox2->Text = L"Red";
			// 
			// ControlTab
			// 
			this->ControlTab->Location = System::Drawing::Point(4, 25);
			this->ControlTab->Name = L"ControlTab";
			this->ControlTab->Size = System::Drawing::Size(987, 127);
			this->ControlTab->TabIndex = 2;
			this->ControlTab->Text = L"Control";
			this->ControlTab->UseVisualStyleBackColor = true;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1012, 845);
			this->Controls->Add(this->MainTabControl);
			this->Controls->Add(this->GLFrame);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->Text = L"Lab1. 3D objects in OpenGL.";
			this->Shown += gcnew System::EventHandler(this, &MyForm::MyForm_Shown);
			this->MainTabControl->ResumeLayout(false);
			this->CameraTab->ResumeLayout(false);
			this->CameraTab->PerformLayout();
			this->AddShapeTab->ResumeLayout(false);
			this->AddShapeTab->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	// Functions and Events
	private: void InitializeGL()
	{
		FrameWidth = GLFrame->Width;
		FrameHeight = GLFrame->Height;
		FrameDepth = GLFrame->Height;
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
	private: System::Void GLFrame_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
		switch (e->KeyChar)
		{
		case 'a': { GL::Translate(1.0, 0.0, 0.0); } break;
		case 'd': { GL::Translate(-1.0, 0.0, 0.0); } break;
		case 'w': { GL::Translate(0.0, 1.0, 0.0); } break;
		case 's': { GL::Translate(0.0, -1.0, 0.0); } break;
		case 'z': { GL::Translate(0.0, 0.0, 1.0); } break;
		case 'c': { GL::Translate(0.0, 0.0, -1.0); } break;

		case 'q': { GL::Scale(0.9, 0.9, 0.9); } break;
		case 'e': { GL::Scale(1.1, 1.1, 1.1); } break;

		case 'r': { GL::Rotate(1, 0.0, 0.0, 1.0); } break;
		case 'f': { GL::Rotate(-1, 0.0, 0.0, 1.0); } break;
		case 't': { GL::Rotate(1, 0.0, 1.0, 0.0); } break;
		case 'g': { GL::Rotate(-1, 0.0, 1.0, 0.0); } break;
		case 'y': { GL::Rotate(1, 1.0, 0.0, 0.0); } break;
		case 'h': { GL::Rotate(-1, 1.0, 0.0, 0.0); } break;
		}
		DrawOrthLines();
		engine.DrawShapes();
		GLFrame->SwapBuffers();
	}
	private: System::Void but_AddShape_Click(System::Object^  sender, System::EventArgs^  e)
	{
		SystemSounds::Beep->Play();
		ShapeType type;
		if (ShapeListBox2->SelectedItem == ShapeListBox2->Items[0]) type = ShapeType::Cube;
		else if (ShapeListBox2->SelectedItem == ShapeListBox2->Items[1]) type = ShapeType::Pyramid;

		Color col;
		if (ColorBox2->SelectedItem == ColorBox2->Items[0]) col = Color::Red;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[1]) col = Color::Orange;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[2]) col = Color::Yellow;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[3]) col = Color::Green;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[4]) col = Color::Aqua;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[5]) col = Color::Blue;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[6]) col = Color::Purple;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[7]) col = Color::Brown;
		else if (ColorBox2->SelectedItem == ColorBox2->Items[8]) col = Color::Gray;

		engine.CreateShape(type, col, Convert::ToUInt32(tb2_leght->Text),
									  Convert::ToDouble(tb2_Xcoor->Text),
								  	  Convert::ToDouble(tb2_Ycoor->Text),
									  Convert::ToDouble(tb2_Zcoor->Text),
									  Convert::ToDouble(tb2_Xrot->Text),
									  Convert::ToDouble(tb2_Yrot->Text),
									  Convert::ToDouble(tb2_Zrot->Text),
									  Convert::ToDouble(tb2_angle->Text));
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
		DrawOrthLines();
		engine.DrawShapes();
		GLFrame->SwapBuffers();
	}

	private: System::Void tb2_leght_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_leght->Text->Length == 0 || tb2_leght->Text == "-") tb2_leght->Text = "0";
	}
	private: System::Void tb2_Xcoor_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_Xcoor->Text->Length == 0 || tb2_Xcoor->Text == "-") tb2_Xcoor->Text = "0";
	}
	private: System::Void tb2_Ycoor_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_Ycoor->Text->Length == 0 || tb2_Ycoor->Text == "-") tb2_Ycoor->Text = "0";
	}
	private: System::Void tb2_Zcoor_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_Zcoor->Text->Length == 0 || tb2_Zcoor->Text == "-") tb2_Zcoor->Text = "0";
	}
	private: System::Void tb2_angle_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_angle->Text->Length == 0 || tb2_angle->Text == "-") tb2_angle->Text = "0";
	}
	private: System::Void tb2_Xrot_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_Xrot->Text->Length == 0 || tb2_Xrot->Text == "-") tb2_Xrot->Text = "0";
	}
	private: System::Void tb2_Yrot_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_Yrot->Text->Length == 0 || tb2_Yrot->Text == "-") tb2_Yrot->Text = "0";
	}
	private: System::Void tb2_Zrot_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_Zrot->Text->Length == 0 || tb2_Zrot->Text == "-") tb2_Zrot->Text = "0";
	}

	private: System::Void tb2_leght_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
			e->Handled = true;
	}
	private: System::Void tb2_Xcoor_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb2_Xcoor->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb2_Ycoor_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb2_Ycoor->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb2_Zcoor_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb2_Zcoor->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb2_angle_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb2_angle->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb2_Xrot_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb2_Xrot->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb2_Yrot_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb2_Yrot->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb2_Zrot_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb2_Zrot->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
};
}