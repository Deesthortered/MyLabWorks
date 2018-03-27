#pragma once

namespace Lab1 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Data::OleDb;
	using namespace System::Drawing;
	using namespace System::Media;
	using namespace OpenTK::Platform::Windows;
	using namespace OpenTK::Graphics::OpenGL;

	int FrameHeight;
	int FrameWidth;
	int FrameDepth;
	size_t cnt_shapes = 1;

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
			size_t id;
			bool selected;
			double x_coor, y_coor, z_coor;
			double x_rot, y_rot, z_rot;
			size_t l;
			Color  color;
		public:
			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id)
			{
				this->id = id;
				this->selected = false;
				this->color = color;
				this->l = l;
				this->x_coor = x_coor;
				this->y_coor = y_coor;
				this->z_coor = z_coor;
				this->x_rot = x_rot;
				this->y_rot = y_rot;
				this->z_rot = z_rot;
			}
			virtual void Draw()
			{
			}
			size_t GetID()
			{
				return this->id;
			}
			void Select()
			{
				this->selected = !this->selected;
			}
			void Deselect()
			{
				this->selected = false;
			}
		};
		ref class Cube : public Shape
		{
			double *x, *y, *z;
			const size_t cnt = 8;
		public:
			Cube()
			{
				x = y = z = nullptr;
			}
			~Cube()
			{
				delete[] x, y, z;
			}
			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id) override
			{
				Shape::SetData(color, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, id);
				x = new double[cnt];
				y = new double[cnt];
				z = new double[cnt];
				x[0] = x[1] = x[4] = x[5] = x_coor - l / 2;
				x[2] = x[3] = x[6] = x[7] = x_coor + l / 2;
				y[0] = y[1] = y[2] = y[3] = y_coor - l / 2;
				y[4] = y[5] = y[6] = y[7] = y_coor + l / 2;
				z[0] = z[3] = z[4] = z[7] = z_coor - l / 2;
				z[1] = z[2] = z[5] = z[6] = z_coor + l / 2;

				double *_x = new double[cnt];
				double *_y = new double[cnt];
				double *_z = new double[cnt];

				for (size_t i = 0; i < cnt; i++) _x[i] = x[i], _y[i] = y[i], _z[i] = z[i];
				for (size_t i = 0; i < cnt; i++)
				{
					_y[i] = y[i] * Math::Cos(x_rot) - z[i] * Math::Sin(x_rot);
					_z[i] = y[i] * Math::Sin(x_rot) + z[i] * Math::Cos(x_rot);
				}
				for (size_t i = 0; i < cnt; i++) x[i] = _x[i], y[i] = _y[i], z[i] = _z[i];
				for (size_t i = 0; i < cnt; i++)
				{
					_x[i] = x[i] * Math::Cos(y_rot) + z[i] * Math::Sin(y_rot);
					_z[i] = -x[i] * Math::Sin(y_rot) + z[i] * Math::Cos(y_rot);
				}
				for (size_t i = 0; i < cnt; i++) x[i] = _x[i], y[i] = _y[i], z[i] = _z[i];
				for (size_t i = 0; i < cnt; i++)
				{
					_x[i] = x[i] * Math::Cos(z_rot) - y[i] * Math::Sin(z_rot);
					_y[i] = x[i] * Math::Sin(z_rot) + y[i] * Math::Cos(z_rot);
				}
				for (size_t i = 0; i < cnt; i++) x[i] = _x[i], y[i] = _y[i], z[i] = _z[i];
				delete[] _x, _y, _z;
			}
			virtual void Draw() override
			{
				Shape::Draw();
				GL::PushName(this->id);
				GL::Begin(BeginMode::Quads);
				if (this->selected) GL::Color3(Color::White);
				else GL::Color3(color);
				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[1], y[1], z[1]);
				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[3], y[3], z[3]);

				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[7], y[7], z[7]);
				GL::Vertex3(x[6], y[6], z[6]);

				GL::Vertex3(x[7], y[7], z[7]);
				GL::Vertex3(x[6], y[6], z[6]);
				GL::Vertex3(x[5], y[5], z[5]);
				GL::Vertex3(x[4], y[4], z[4]);

				GL::Vertex3(x[5], y[5], z[5]);
				GL::Vertex3(x[4], y[4], z[4]);
				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[1], y[1], z[1]);

				GL::Vertex3(x[1], y[1], z[1]);
				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[6], y[6], z[6]);
				GL::Vertex3(x[5], y[5], z[5]);

				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[7], y[7], z[7]);
				GL::Vertex3(x[4], y[4], z[4]);
				GL::End();

				GL::Color3(Color::White);
				GL::Begin(BeginMode::LineStrip);
				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[1], y[1], z[1]);
				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[0], y[0], z[0]);
				GL::End();
				GL::Begin(BeginMode::LineStrip);
				GL::Vertex3(x[4], y[4], z[4]);
				GL::Vertex3(x[5], y[5], z[5]);
				GL::Vertex3(x[6], y[6], z[6]);
				GL::Vertex3(x[7], y[7], z[7]);
				GL::Vertex3(x[4], y[4], z[4]);
				GL::End();
				GL::Begin(BeginMode::Lines);
				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[4], y[4], z[4]);
				GL::Vertex3(x[1], y[1], z[1]);
				GL::Vertex3(x[5], y[5], z[5]);
				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[6], y[6], z[6]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[7], y[7], z[7]);
				GL::End();
				GL::PopName();
			}
		};
		ref class Pyramid : public Shape
		{
			double *x, *y, *z;
			const size_t cnt = 4;
		public:
			Pyramid()
			{
				x = y = z = nullptr;
			}
			~Pyramid()
			{
				delete[] x, y, z;
			}

			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id) override
			{
				Shape::SetData(color, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, id);
				x = new double[cnt];
				y = new double[cnt];
				z = new double[cnt];
				x[0] = x[2] = x_coor - (l / Math::Sqrt(12)); x[1] = x_coor + (l / Math::Sqrt(3)); x[3] = x_coor;
				y[0] = y[1] = y[2] = y_coor; y[3] = y_coor + Math::Sqrt((2 * l*l) / 3);
				z[0] = z_coor - (l / 2); z[1] = z[3] = z_coor; z[2] = z_coor + (l / 2);

				double *_x = new double[cnt];
				double *_y = new double[cnt];
				double *_z = new double[cnt];

				for (size_t i = 0; i < cnt; i++) _x[i] = x[i], _y[i] = y[i], _z[i] = z[i];
				for (size_t i = 0; i < cnt; i++)
				{
					_y[i] = y[i] * Math::Cos(x_rot) - z[i] * Math::Sin(x_rot);
					_z[i] = y[i] * Math::Sin(x_rot) + z[i] * Math::Cos(x_rot);
				}
				for (size_t i = 0; i < cnt; i++) x[i] = _x[i], y[i] = _y[i], z[i] = _z[i];
				for (size_t i = 0; i < cnt; i++)
				{
					_x[i] = x[i] * Math::Cos(y_rot) + z[i] * Math::Sin(y_rot);
					_z[i] = -x[i] * Math::Sin(y_rot) + z[i] * Math::Cos(y_rot);
				}
				for (size_t i = 0; i < cnt; i++) x[i] = _x[i], y[i] = _y[i], z[i] = _z[i];
				for (size_t i = 0; i < cnt; i++)
				{
					_x[i] = x[i] * Math::Cos(z_rot) - y[i] * Math::Sin(z_rot);
					_y[i] = x[i] * Math::Sin(z_rot) + y[i] * Math::Cos(z_rot);
				}
				for (size_t i = 0; i < cnt; i++) x[i] = _x[i], y[i] = _y[i], z[i] = _z[i];
				delete[] _x, _y, _z;
			}
			virtual void Draw() override
			{
				Shape::Draw();
				GL::PushName(this->id);
				GL::Begin(BeginMode::Triangles);
				if (this->selected) GL::Color3(Color::White);
				else GL::Color3(color);

				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[1], y[1], z[1]);
				GL::Vertex3(x[2], y[2], z[2]);

				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[2], y[2], z[2]);

				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[1], y[1], z[1]);

				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[1], y[1], z[1]);
				GL::End();

				GL::Color3(Color::White);
				GL::Begin(BeginMode::LineStrip);
				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[1], y[1], z[1]);
				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[0], y[0], z[0]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::Vertex3(x[2], y[2], z[2]);
				GL::Vertex3(x[1], y[1], z[1]);
				GL::Vertex3(x[3], y[3], z[3]);
				GL::End();
				GL::PopName();
			}
		};

		ref class Engine
		{
			List<Shape^> shape_list;
			double TranslateStep;
			double ScaleStep;
			double RotareStep;
			float *light_pos;
		public:
			Engine()
			{
				TranslateStep = 1;
				ScaleStep = 0.1;
				RotareStep = 1;
				light_pos = new float[4];
				light_pos[0] = 100;
				light_pos[1] = 100;
				light_pos[2] = 100;
				light_pos[3] = 1;
			}
			~Engine()
			{
				//delete[] light_pos;
			}

			void SetTranslateStep(double val)
			{
				this->TranslateStep = val;
			}
			void SetScaleStep(double val)
			{
				this->ScaleStep = val;
			}
			void SetRotareStep(double val)
			{
				this->RotareStep = val;
			}

			void CreateShape(ShapeType type, Color col, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id)
			{
				Shape^ s;
				if (type == ShapeType::Cube) s = gcnew Cube();
				else s = gcnew Pyramid();
				s->SetData(col, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, id);
				this->shape_list.Add(s);
			}
			void DeleteShape(size_t id)
			{
				for (int i = 0; i < shape_list.Count; i++)
					if (shape_list[i]->GetID() == id)
					{
						shape_list.RemoveAt(i);
						return;
					}
			}
			void DrawShapes()
			{
				for each (Shape^ cur in shape_list)
				{
					cur->Draw();
				}
				GL::Light(LightName::Light0, LightParameter::Position, light_pos);
			}

			void CameraControl(System::Windows::Forms::KeyPressEventArgs^  &e)
			{
				switch (e->KeyChar)
				{
				case 'a': { GL::Translate(TranslateStep, 0.0, 0.0); } break;
				case 'd': { GL::Translate(-TranslateStep, 0.0, 0.0); } break;
				case 'w': { GL::Translate(0.0, TranslateStep, 0.0); } break;
				case 's': { GL::Translate(0.0, -TranslateStep, 0.0); } break;
				case 'z': { GL::Translate(0.0, 0.0, TranslateStep); } break;
				case 'c': { GL::Translate(0.0, 0.0, -TranslateStep); } break;

				case 'q': { GL::Scale(1.0 - ScaleStep, 1.0 - ScaleStep, 1.0 - ScaleStep); } break;
				case 'e': { GL::Scale(1.0 + ScaleStep, 1.0 + ScaleStep, 1.0 + ScaleStep); } break;

				case 'r': { GL::Rotate(RotareStep, 1.0, 0.0, 0.0); } break;
				case 'f': { GL::Rotate(-RotareStep, 1.0, 0.0, 0.0); } break;
				case 't': { GL::Rotate(RotareStep, 0.0, 1.0, 0.0); } break;
				case 'g': { GL::Rotate(-RotareStep, 0.0, 1.0, 0.0); } break;
				case 'y': { GL::Rotate(RotareStep, 0.0, 0.0, 1.0); } break;
				case 'h': { GL::Rotate(-RotareStep, 0.0, 0.0, 1.0); } break;

				case 'u': { light_pos[0] += (float)TranslateStep; } break;
				case 'j': { light_pos[0] -= (float)TranslateStep; } break;
				case 'i': { light_pos[1] += (float)TranslateStep; } break;
				case 'k': { light_pos[1] -= (float)TranslateStep; } break;
				case 'o': { light_pos[2] += (float)TranslateStep; } break;
				case 'l': { light_pos[2] -= (float)TranslateStep; } break;
				}
			}
			void NullLightPos()
			{
				light_pos[0] = 0;
				light_pos[1] = 0;
				light_pos[2] = 0;
			}

			float GetLightX()
			{
				return light_pos[0];
			}
			float GetLightY()
			{
				return light_pos[1];
			}
			float GetLightZ()
			{
				return light_pos[2];
			}

			void SelectShape(size_t id)
			{
				for (int i = 0; i < shape_list.Count; i++)
					if (shape_list[i]->GetID() == id)
					{
						shape_list[i]->Select();
						return;
					}
			}
			void DeselectAll()
			{
				for (int i = 0; i < shape_list.Count; i++)
					shape_list[i]->Deselect();
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
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::TabControl^  MainTabControl;
	private: System::Windows::Forms::TabPage^  CameraTab;
	private: System::Windows::Forms::TabPage^  AddShapeTab;
	private: System::Windows::Forms::Label^  label2Color;
	private: System::Windows::Forms::ComboBox^  ColorBox2;
	private: System::Windows::Forms::Label^  label1rot;
	private: System::Windows::Forms::Label^  label1scal;
	private: System::Windows::Forms::Label^  label1trans;
	private: System::Windows::Forms::TextBox^  tb1_scal;
	private: System::Windows::Forms::TextBox^  tb1_rot;
	private: System::Windows::Forms::TextBox^  tb1_trans;
	private: System::Windows::Forms::TabPage^  ControlTab;
	private: System::Windows::Forms::CheckBox^  cb1_axisZ;
	private: System::Windows::Forms::CheckBox^  cb1_axisY;
	private: System::Windows::Forms::CheckBox^  cb1_axisX;
	private: System::Windows::Forms::Label^  label1Planes;
	private: System::Windows::Forms::Label^  label1Axises;
	private: System::Windows::Forms::CheckBox^  cb1_planeXZ;
	private: System::Windows::Forms::CheckBox^  cb1_planeXY;
	private: System::Windows::Forms::CheckBox^  cb1_planeYZ;
	private: System::Windows::Forms::RichTextBox^  rtb1_manual;
	private: System::Windows::Forms::Button^  but1LightNull;
	private: System::Windows::Forms::TextBox^  tb1_LightZ;
	private: System::Windows::Forms::TextBox^  tb1_LightY;
	private: System::Windows::Forms::TextBox^  tb1_LightX;
	private: System::Windows::Forms::Label^  label1LightZ;
	private: System::Windows::Forms::Label^  label1LightY;
	private: System::Windows::Forms::Label^  label1LightX;
	private: System::Windows::Forms::Label^  label1LightPos;

	private: System::Windows::Forms::DataGridView^  dataGridView;
	private: System::Data::OleDb::OleDbCommand^  oleDbSelectCommand1;
	private: System::Data::OleDb::OleDbConnection^  oleDbConnection1;
	private: System::Data::OleDb::OleDbCommand^  oleDbInsertCommand1;
	private: System::Data::OleDb::OleDbCommand^  oleDbUpdateCommand1;
	private: System::Data::OleDb::OleDbCommand^  oleDbDeleteCommand1;
	private: System::Data::OleDb::OleDbDataAdapter^  oleDbDataAdapter1;
	private: System::Data::DataSet^  dataSet1;


	private: Engine engine;
#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
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
			this->but1LightNull = (gcnew System::Windows::Forms::Button());
			this->tb1_LightZ = (gcnew System::Windows::Forms::TextBox());
			this->tb1_LightY = (gcnew System::Windows::Forms::TextBox());
			this->tb1_LightX = (gcnew System::Windows::Forms::TextBox());
			this->label1LightZ = (gcnew System::Windows::Forms::Label());
			this->label1LightY = (gcnew System::Windows::Forms::Label());
			this->label1LightX = (gcnew System::Windows::Forms::Label());
			this->label1LightPos = (gcnew System::Windows::Forms::Label());
			this->rtb1_manual = (gcnew System::Windows::Forms::RichTextBox());
			this->cb1_planeYZ = (gcnew System::Windows::Forms::CheckBox());
			this->cb1_planeXZ = (gcnew System::Windows::Forms::CheckBox());
			this->cb1_planeXY = (gcnew System::Windows::Forms::CheckBox());
			this->cb1_axisZ = (gcnew System::Windows::Forms::CheckBox());
			this->cb1_axisY = (gcnew System::Windows::Forms::CheckBox());
			this->cb1_axisX = (gcnew System::Windows::Forms::CheckBox());
			this->label1Planes = (gcnew System::Windows::Forms::Label());
			this->label1Axises = (gcnew System::Windows::Forms::Label());
			this->tb1_scal = (gcnew System::Windows::Forms::TextBox());
			this->tb1_rot = (gcnew System::Windows::Forms::TextBox());
			this->tb1_trans = (gcnew System::Windows::Forms::TextBox());
			this->label1rot = (gcnew System::Windows::Forms::Label());
			this->label1scal = (gcnew System::Windows::Forms::Label());
			this->label1trans = (gcnew System::Windows::Forms::Label());
			this->AddShapeTab = (gcnew System::Windows::Forms::TabPage());
			this->label2Color = (gcnew System::Windows::Forms::Label());
			this->ColorBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->ControlTab = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->dataSet1 = (gcnew System::Data::DataSet());
			this->oleDbSelectCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbConnection1 = (gcnew System::Data::OleDb::OleDbConnection());
			this->oleDbInsertCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbUpdateCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbDeleteCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbDataAdapter1 = (gcnew System::Data::OleDb::OleDbDataAdapter());
			this->MainTabControl->SuspendLayout();
			this->CameraTab->SuspendLayout();
			this->AddShapeTab->SuspendLayout();
			this->ControlTab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataSet1))->BeginInit();
			this->SuspendLayout();
			// 
			// GLFrame
			// 
			this->GLFrame->BackColor = System::Drawing::Color::Black;
			this->GLFrame->Cursor = System::Windows::Forms::Cursors::NoMove2D;
			this->GLFrame->Location = System::Drawing::Point(7, 171);
			this->GLFrame->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->GLFrame->Name = L"GLFrame";
			this->GLFrame->Size = System::Drawing::Size(990, 660);
			this->GLFrame->TabIndex = 0;
			this->GLFrame->VSync = false;
			this->GLFrame->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::GLFrame_KeyPress);
			this->GLFrame->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::GLFrame_MouseDown);
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
			this->tb2_leght->Text = L"50";
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
			this->tb2_Xcoor->Click += gcnew System::EventHandler(this, &MyForm::tb2_Xcoor_Click);
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
			this->tb2_Ycoor->Click += gcnew System::EventHandler(this, &MyForm::tb2_Ycoor_Click);
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
			this->tb2_Zcoor->Click += gcnew System::EventHandler(this, &MyForm::tb2_Zcoor_Click);
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
			this->label2XRot->Location = System::Drawing::Point(391, 29);
			this->label2XRot->Name = L"label2XRot";
			this->label2XRot->Size = System::Drawing::Size(29, 17);
			this->label2XRot->TabIndex = 12;
			this->label2XRot->Text = L"X =";
			// 
			// label2YRot
			// 
			this->label2YRot->AutoSize = true;
			this->label2YRot->Location = System::Drawing::Point(391, 57);
			this->label2YRot->Name = L"label2YRot";
			this->label2YRot->Size = System::Drawing::Size(29, 17);
			this->label2YRot->TabIndex = 13;
			this->label2YRot->Text = L"Y =";
			// 
			// label2ZRot
			// 
			this->label2ZRot->AutoSize = true;
			this->label2ZRot->Location = System::Drawing::Point(391, 85);
			this->label2ZRot->Name = L"label2ZRot";
			this->label2ZRot->Size = System::Drawing::Size(29, 17);
			this->label2ZRot->TabIndex = 14;
			this->label2ZRot->Text = L"Z =";
			// 
			// tb2_Xrot
			// 
			this->tb2_Xrot->Location = System::Drawing::Point(426, 26);
			this->tb2_Xrot->Name = L"tb2_Xrot";
			this->tb2_Xrot->Size = System::Drawing::Size(46, 22);
			this->tb2_Xrot->TabIndex = 15;
			this->tb2_Xrot->Text = L"0";
			this->tb2_Xrot->Click += gcnew System::EventHandler(this, &MyForm::tb2_Xrot_Click);
			this->tb2_Xrot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Xrot_KeyPress);
			this->tb2_Xrot->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Xrot_Leave);
			// 
			// tb2_Yrot
			// 
			this->tb2_Yrot->Location = System::Drawing::Point(426, 54);
			this->tb2_Yrot->Name = L"tb2_Yrot";
			this->tb2_Yrot->Size = System::Drawing::Size(46, 22);
			this->tb2_Yrot->TabIndex = 16;
			this->tb2_Yrot->Text = L"0";
			this->tb2_Yrot->Click += gcnew System::EventHandler(this, &MyForm::tb2_Yrot_Click);
			this->tb2_Yrot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb2_Yrot_KeyPress);
			this->tb2_Yrot->Leave += gcnew System::EventHandler(this, &MyForm::tb2_Yrot_Leave);
			// 
			// tb2_Zrot
			// 
			this->tb2_Zrot->Location = System::Drawing::Point(426, 82);
			this->tb2_Zrot->Name = L"tb2_Zrot";
			this->tb2_Zrot->Size = System::Drawing::Size(46, 22);
			this->tb2_Zrot->TabIndex = 17;
			this->tb2_Zrot->Text = L"0";
			this->tb2_Zrot->Click += gcnew System::EventHandler(this, &MyForm::tb2_Zrot_Click);
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
			this->CameraTab->Controls->Add(this->but1LightNull);
			this->CameraTab->Controls->Add(this->tb1_LightZ);
			this->CameraTab->Controls->Add(this->tb1_LightY);
			this->CameraTab->Controls->Add(this->tb1_LightX);
			this->CameraTab->Controls->Add(this->label1LightZ);
			this->CameraTab->Controls->Add(this->label1LightY);
			this->CameraTab->Controls->Add(this->label1LightX);
			this->CameraTab->Controls->Add(this->label1LightPos);
			this->CameraTab->Controls->Add(this->rtb1_manual);
			this->CameraTab->Controls->Add(this->cb1_planeYZ);
			this->CameraTab->Controls->Add(this->cb1_planeXZ);
			this->CameraTab->Controls->Add(this->cb1_planeXY);
			this->CameraTab->Controls->Add(this->cb1_axisZ);
			this->CameraTab->Controls->Add(this->cb1_axisY);
			this->CameraTab->Controls->Add(this->cb1_axisX);
			this->CameraTab->Controls->Add(this->label1Planes);
			this->CameraTab->Controls->Add(this->label1Axises);
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
			// but1LightNull
			// 
			this->but1LightNull->Location = System::Drawing::Point(890, 31);
			this->but1LightNull->Name = L"but1LightNull";
			this->but1LightNull->Size = System::Drawing::Size(82, 76);
			this->but1LightNull->TabIndex = 24;
			this->but1LightNull->Text = L"Set light to null";
			this->but1LightNull->UseVisualStyleBackColor = true;
			this->but1LightNull->Click += gcnew System::EventHandler(this, &MyForm::but1LightNull_Click);
			// 
			// tb1_LightZ
			// 
			this->tb1_LightZ->Location = System::Drawing::Point(814, 87);
			this->tb1_LightZ->Name = L"tb1_LightZ";
			this->tb1_LightZ->ReadOnly = true;
			this->tb1_LightZ->Size = System::Drawing::Size(70, 22);
			this->tb1_LightZ->TabIndex = 23;
			this->tb1_LightZ->Text = L"100";
			// 
			// tb1_LightY
			// 
			this->tb1_LightY->Location = System::Drawing::Point(814, 59);
			this->tb1_LightY->Name = L"tb1_LightY";
			this->tb1_LightY->ReadOnly = true;
			this->tb1_LightY->Size = System::Drawing::Size(70, 22);
			this->tb1_LightY->TabIndex = 22;
			this->tb1_LightY->Text = L"100";
			// 
			// tb1_LightX
			// 
			this->tb1_LightX->Location = System::Drawing::Point(814, 31);
			this->tb1_LightX->Name = L"tb1_LightX";
			this->tb1_LightX->ReadOnly = true;
			this->tb1_LightX->Size = System::Drawing::Size(70, 22);
			this->tb1_LightX->TabIndex = 21;
			this->tb1_LightX->Text = L"100";
			// 
			// label1LightZ
			// 
			this->label1LightZ->AutoSize = true;
			this->label1LightZ->Location = System::Drawing::Point(792, 90);
			this->label1LightZ->Name = L"label1LightZ";
			this->label1LightZ->Size = System::Drawing::Size(25, 17);
			this->label1LightZ->TabIndex = 20;
			this->label1LightZ->Text = L"Z :";
			// 
			// label1LightY
			// 
			this->label1LightY->AutoSize = true;
			this->label1LightY->Location = System::Drawing::Point(791, 64);
			this->label1LightY->Name = L"label1LightY";
			this->label1LightY->Size = System::Drawing::Size(25, 17);
			this->label1LightY->TabIndex = 19;
			this->label1LightY->Text = L"Y :";
			// 
			// label1LightX
			// 
			this->label1LightX->AutoSize = true;
			this->label1LightX->Location = System::Drawing::Point(792, 36);
			this->label1LightX->Name = L"label1LightX";
			this->label1LightX->Size = System::Drawing::Size(25, 17);
			this->label1LightX->TabIndex = 18;
			this->label1LightX->Text = L"X :";
			// 
			// label1LightPos
			// 
			this->label1LightPos->AutoSize = true;
			this->label1LightPos->Location = System::Drawing::Point(777, 12);
			this->label1LightPos->Name = L"label1LightPos";
			this->label1LightPos->Size = System::Drawing::Size(96, 17);
			this->label1LightPos->TabIndex = 17;
			this->label1LightPos->Text = L"Light position:";
			// 
			// rtb1_manual
			// 
			this->rtb1_manual->Location = System::Drawing::Point(357, 12);
			this->rtb1_manual->Name = L"rtb1_manual";
			this->rtb1_manual->ReadOnly = true;
			this->rtb1_manual->Size = System::Drawing::Size(413, 109);
			this->rtb1_manual->TabIndex = 16;
			this->rtb1_manual->Text = resources->GetString(L"rtb1_manual.Text");
			// 
			// cb1_planeYZ
			// 
			this->cb1_planeYZ->AutoSize = true;
			this->cb1_planeYZ->Location = System::Drawing::Point(299, 86);
			this->cb1_planeYZ->Name = L"cb1_planeYZ";
			this->cb1_planeYZ->Size = System::Drawing::Size(48, 21);
			this->cb1_planeYZ->TabIndex = 15;
			this->cb1_planeYZ->Text = L"YZ";
			this->cb1_planeYZ->UseVisualStyleBackColor = true;
			this->cb1_planeYZ->Click += gcnew System::EventHandler(this, &MyForm::cb1_planeYZ_Click);
			// 
			// cb1_planeXZ
			// 
			this->cb1_planeXZ->AutoSize = true;
			this->cb1_planeXZ->Location = System::Drawing::Point(299, 60);
			this->cb1_planeXZ->Name = L"cb1_planeXZ";
			this->cb1_planeXZ->Size = System::Drawing::Size(48, 21);
			this->cb1_planeXZ->TabIndex = 14;
			this->cb1_planeXZ->Text = L"XZ";
			this->cb1_planeXZ->UseVisualStyleBackColor = true;
			this->cb1_planeXZ->Click += gcnew System::EventHandler(this, &MyForm::cb1_planeXZ_Click);
			// 
			// cb1_planeXY
			// 
			this->cb1_planeXY->AutoSize = true;
			this->cb1_planeXY->Location = System::Drawing::Point(299, 32);
			this->cb1_planeXY->Name = L"cb1_planeXY";
			this->cb1_planeXY->Size = System::Drawing::Size(48, 21);
			this->cb1_planeXY->TabIndex = 13;
			this->cb1_planeXY->Text = L"XY";
			this->cb1_planeXY->UseVisualStyleBackColor = true;
			this->cb1_planeXY->Click += gcnew System::EventHandler(this, &MyForm::cb1_planeXY_Click);
			// 
			// cb1_axisZ
			// 
			this->cb1_axisZ->AutoSize = true;
			this->cb1_axisZ->Checked = true;
			this->cb1_axisZ->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cb1_axisZ->Location = System::Drawing::Point(237, 86);
			this->cb1_axisZ->Name = L"cb1_axisZ";
			this->cb1_axisZ->Size = System::Drawing::Size(39, 21);
			this->cb1_axisZ->TabIndex = 12;
			this->cb1_axisZ->Text = L"Z";
			this->cb1_axisZ->UseVisualStyleBackColor = true;
			this->cb1_axisZ->Click += gcnew System::EventHandler(this, &MyForm::cb1_axisZ_Click);
			// 
			// cb1_axisY
			// 
			this->cb1_axisY->AutoSize = true;
			this->cb1_axisY->Checked = true;
			this->cb1_axisY->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cb1_axisY->Location = System::Drawing::Point(237, 59);
			this->cb1_axisY->Name = L"cb1_axisY";
			this->cb1_axisY->Size = System::Drawing::Size(39, 21);
			this->cb1_axisY->TabIndex = 11;
			this->cb1_axisY->Text = L"Y";
			this->cb1_axisY->UseVisualStyleBackColor = true;
			this->cb1_axisY->Click += gcnew System::EventHandler(this, &MyForm::cb1_axisY_Click);
			// 
			// cb1_axisX
			// 
			this->cb1_axisX->AutoSize = true;
			this->cb1_axisX->Checked = true;
			this->cb1_axisX->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cb1_axisX->Location = System::Drawing::Point(237, 32);
			this->cb1_axisX->Name = L"cb1_axisX";
			this->cb1_axisX->Size = System::Drawing::Size(39, 21);
			this->cb1_axisX->TabIndex = 10;
			this->cb1_axisX->Text = L"X";
			this->cb1_axisX->UseVisualStyleBackColor = true;
			this->cb1_axisX->Click += gcnew System::EventHandler(this, &MyForm::cb1_axisX_Click);
			// 
			// label1Planes
			// 
			this->label1Planes->AutoSize = true;
			this->label1Planes->Location = System::Drawing::Point(296, 12);
			this->label1Planes->Name = L"label1Planes";
			this->label1Planes->Size = System::Drawing::Size(55, 17);
			this->label1Planes->TabIndex = 9;
			this->label1Planes->Text = L"Planes:";
			// 
			// label1Axises
			// 
			this->label1Axises->AutoSize = true;
			this->label1Axises->Location = System::Drawing::Point(234, 12);
			this->label1Axises->Name = L"label1Axises";
			this->label1Axises->Size = System::Drawing::Size(52, 17);
			this->label1Axises->TabIndex = 8;
			this->label1Axises->Text = L"Axises:";
			// 
			// tb1_scal
			// 
			this->tb1_scal->Location = System::Drawing::Point(115, 70);
			this->tb1_scal->Name = L"tb1_scal";
			this->tb1_scal->Size = System::Drawing::Size(100, 22);
			this->tb1_scal->TabIndex = 5;
			this->tb1_scal->Text = L"0,1";
			this->tb1_scal->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb1_scal_KeyPress);
			this->tb1_scal->Leave += gcnew System::EventHandler(this, &MyForm::tb1_scal_Leave);
			// 
			// tb1_rot
			// 
			this->tb1_rot->Location = System::Drawing::Point(115, 41);
			this->tb1_rot->Name = L"tb1_rot";
			this->tb1_rot->Size = System::Drawing::Size(100, 22);
			this->tb1_rot->TabIndex = 4;
			this->tb1_rot->Text = L"1";
			this->tb1_rot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb1_rot_KeyPress);
			this->tb1_rot->Leave += gcnew System::EventHandler(this, &MyForm::tb1_rot_Leave);
			// 
			// tb1_trans
			// 
			this->tb1_trans->Location = System::Drawing::Point(115, 12);
			this->tb1_trans->Name = L"tb1_trans";
			this->tb1_trans->Size = System::Drawing::Size(100, 22);
			this->tb1_trans->TabIndex = 3;
			this->tb1_trans->Text = L"1";
			this->tb1_trans->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb1_trans_KeyPress);
			this->tb1_trans->Leave += gcnew System::EventHandler(this, &MyForm::tb1_trans_Leave);
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
			this->ControlTab->Controls->Add(this->dataGridView);
			this->ControlTab->Location = System::Drawing::Point(4, 25);
			this->ControlTab->Name = L"ControlTab";
			this->ControlTab->Size = System::Drawing::Size(987, 127);
			this->ControlTab->TabIndex = 2;
			this->ControlTab->Text = L"Control";
			this->ControlTab->UseVisualStyleBackColor = true;
			// 
			// dataGridView
			// 
			this->dataGridView->AllowUserToAddRows = false;
			this->dataGridView->AutoGenerateColumns = false;
			this->dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView->DataSource = this->dataSet1;
			this->dataGridView->Location = System::Drawing::Point(3, 3);
			this->dataGridView->Name = L"dataGridView";
			this->dataGridView->ReadOnly = true;
			this->dataGridView->RowTemplate->Height = 24;
			this->dataGridView->Size = System::Drawing::Size(978, 121);
			this->dataGridView->TabIndex = 2;
			this->dataGridView->UserDeletedRow += gcnew System::Windows::Forms::DataGridViewRowEventHandler(this, &MyForm::dataGridView_UserDeletedRow);
			this->dataGridView->UserDeletingRow += gcnew System::Windows::Forms::DataGridViewRowCancelEventHandler(this, &MyForm::dataGridView_UserDeletingRow);
			// 
			// dataSet1
			// 
			this->dataSet1->DataSetName = L"NewDataSet";
			// 
			// oleDbSelectCommand1
			// 
			this->oleDbSelectCommand1->CommandText = L"SELECT Shapes.*\r\nFROM     Shapes";
			this->oleDbSelectCommand1->Connection = this->oleDbConnection1;
			// 
			// oleDbConnection1
			// 
			this->oleDbConnection1->ConnectionString = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=D:\\2.Programming\\MyLabWorks\\Lab1\\dat"
				L"abase.mdb";
			// 
			// oleDbInsertCommand1
			// 
			this->oleDbInsertCommand1->CommandText = L"INSERT INTO `Shapes` (`x_coor`, `y_coor`, `z_coor`, `lenght`, `x_rot`, `y_rot`, `"
				L"z_rot`, `Cube/Pyramid`, `Color`) VALUES (\?, \?, \?, \?, \?, \?, \?, \?, \?)";
			this->oleDbInsertCommand1->Connection = this->oleDbConnection1;
			this->oleDbInsertCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(9) {
				(gcnew System::Data::OleDb::OleDbParameter(L"x_coor",
					System::Data::OleDb::OleDbType::Integer, 0, L"x_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"y_coor", System::Data::OleDb::OleDbType::Integer,
						0, L"y_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"z_coor", System::Data::OleDb::OleDbType::Integer, 0, L"z_coor")),
						(gcnew System::Data::OleDb::OleDbParameter(L"lenght", System::Data::OleDb::OleDbType::Integer, 0, L"lenght")), (gcnew System::Data::OleDb::OleDbParameter(L"x_rot",
							System::Data::OleDb::OleDbType::Integer, 0, L"x_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"y_rot", System::Data::OleDb::OleDbType::Integer,
								0, L"y_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"z_rot", System::Data::OleDb::OleDbType::Integer, 0, L"z_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"Cube/Pyramid",
									System::Data::OleDb::OleDbType::Boolean, 0, L"Cube/Pyramid")), (gcnew System::Data::OleDb::OleDbParameter(L"Color", System::Data::OleDb::OleDbType::Integer,
										0, L"Color"))
			});
			// 
			// oleDbUpdateCommand1
			// 
			this->oleDbUpdateCommand1->CommandText = resources->GetString(L"oleDbUpdateCommand1.CommandText");
			this->oleDbUpdateCommand1->Connection = this->oleDbConnection1;
			this->oleDbUpdateCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(28) {
				(gcnew System::Data::OleDb::OleDbParameter(L"x_coor",
					System::Data::OleDb::OleDbType::Integer, 0, L"x_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"y_coor", System::Data::OleDb::OleDbType::Integer,
						0, L"y_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"z_coor", System::Data::OleDb::OleDbType::Integer, 0, L"z_coor")),
						(gcnew System::Data::OleDb::OleDbParameter(L"lenght", System::Data::OleDb::OleDbType::Integer, 0, L"lenght")), (gcnew System::Data::OleDb::OleDbParameter(L"x_rot",
							System::Data::OleDb::OleDbType::Integer, 0, L"x_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"y_rot", System::Data::OleDb::OleDbType::Integer,
								0, L"y_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"z_rot", System::Data::OleDb::OleDbType::Integer, 0, L"z_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"Cube/Pyramid",
									System::Data::OleDb::OleDbType::Boolean, 0, L"Cube/Pyramid")), (gcnew System::Data::OleDb::OleDbParameter(L"Color", System::Data::OleDb::OleDbType::Integer,
										0, L"Color")), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Код", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
											false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Код", System::Data::DataRowVersion::Original, nullptr)),
											(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_x_coor", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
												static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"x_coor", System::Data::DataRowVersion::Original, true, nullptr)),
												(gcnew System::Data::OleDb::OleDbParameter(L"Original_x_coor", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
													false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"x_coor", System::Data::DataRowVersion::Original, nullptr)),
													(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_y_coor", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
														static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"y_coor", System::Data::DataRowVersion::Original, true, nullptr)),
														(gcnew System::Data::OleDb::OleDbParameter(L"Original_y_coor", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
															false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"y_coor", System::Data::DataRowVersion::Original, nullptr)),
															(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_z_coor", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"z_coor", System::Data::DataRowVersion::Original, true, nullptr)),
																(gcnew System::Data::OleDb::OleDbParameter(L"Original_z_coor", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																	false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"z_coor", System::Data::DataRowVersion::Original, nullptr)),
																	(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_lenght", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																		static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"lenght", System::Data::DataRowVersion::Original, true, nullptr)),
																		(gcnew System::Data::OleDb::OleDbParameter(L"Original_lenght", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																			false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"lenght", System::Data::DataRowVersion::Original, nullptr)),
																			(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_x_rot", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																				static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"x_rot", System::Data::DataRowVersion::Original, true, nullptr)),
																				(gcnew System::Data::OleDb::OleDbParameter(L"Original_x_rot", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																					false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"x_rot", System::Data::DataRowVersion::Original, nullptr)),
																					(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_y_rot", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																						static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"y_rot", System::Data::DataRowVersion::Original, true, nullptr)),
																						(gcnew System::Data::OleDb::OleDbParameter(L"Original_y_rot", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																							false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"y_rot", System::Data::DataRowVersion::Original, nullptr)),
																							(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_z_rot", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																								static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"z_rot", System::Data::DataRowVersion::Original, true, nullptr)),
																								(gcnew System::Data::OleDb::OleDbParameter(L"Original_z_rot", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																									false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"z_rot", System::Data::DataRowVersion::Original, nullptr)),
																									(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Cube/Pyramid", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																										static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Cube/Pyramid", System::Data::DataRowVersion::Original, true,
																										nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Cube/Pyramid", System::Data::OleDb::OleDbType::Boolean,
																											0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Cube/Pyramid",
																											System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color", System::Data::OleDb::OleDbType::Integer,
																												0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color", System::Data::DataRowVersion::Original,
																												true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Color", System::Data::OleDb::OleDbType::Integer, 0,
																													System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color", System::Data::DataRowVersion::Original,
																													nullptr))
			});
			// 
			// oleDbDeleteCommand1
			// 
			this->oleDbDeleteCommand1->CommandText = resources->GetString(L"oleDbDeleteCommand1.CommandText");
			this->oleDbDeleteCommand1->Connection = this->oleDbConnection1;
			this->oleDbDeleteCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(19) {
				(gcnew System::Data::OleDb::OleDbParameter(L"Original_Код",
					System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
					static_cast<System::Byte>(0), L"Код", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_x_coor",
						System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
						L"x_coor", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_x_coor",
							System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
							static_cast<System::Byte>(0), L"x_coor", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_y_coor",
								System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
								L"y_coor", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_y_coor",
									System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
									static_cast<System::Byte>(0), L"y_coor", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_z_coor",
										System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
										L"z_coor", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_z_coor",
											System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
											static_cast<System::Byte>(0), L"z_coor", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_lenght",
												System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
												L"lenght", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_lenght",
													System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
													static_cast<System::Byte>(0), L"lenght", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_x_rot",
														System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
														L"x_rot", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_x_rot",
															System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
															static_cast<System::Byte>(0), L"x_rot", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_y_rot",
																System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																L"y_rot", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_y_rot",
																	System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																	static_cast<System::Byte>(0), L"y_rot", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_z_rot",
																		System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																		L"z_rot", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_z_rot",
																			System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																			static_cast<System::Byte>(0), L"z_rot", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Cube/Pyramid",
																				System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																				L"Cube/Pyramid", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Cube/Pyramid",
																					System::Data::OleDb::OleDbType::Boolean, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																					static_cast<System::Byte>(0), L"Cube/Pyramid", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color",
																						System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																						L"Color", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Color",
																							System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																							static_cast<System::Byte>(0), L"Color", System::Data::DataRowVersion::Original, nullptr))
			});
			// 
			// oleDbDataAdapter1
			// 
			this->oleDbDataAdapter1->DeleteCommand = this->oleDbDeleteCommand1;
			this->oleDbDataAdapter1->InsertCommand = this->oleDbInsertCommand1;
			this->oleDbDataAdapter1->SelectCommand = this->oleDbSelectCommand1;
			cli::array< System::Data::Common::DataColumnMapping^ >^ __mcTemp__1 = gcnew cli::array< System::Data::Common::DataColumnMapping^  >(10) {
				(gcnew System::Data::Common::DataColumnMapping(L"Код",
					L"Код")), (gcnew System::Data::Common::DataColumnMapping(L"x_coor", L"x_coor")), (gcnew System::Data::Common::DataColumnMapping(L"y_coor",
						L"y_coor")), (gcnew System::Data::Common::DataColumnMapping(L"z_coor", L"z_coor")), (gcnew System::Data::Common::DataColumnMapping(L"lenght",
							L"lenght")), (gcnew System::Data::Common::DataColumnMapping(L"x_rot", L"x_rot")), (gcnew System::Data::Common::DataColumnMapping(L"y_rot",
								L"y_rot")), (gcnew System::Data::Common::DataColumnMapping(L"z_rot", L"z_rot")), (gcnew System::Data::Common::DataColumnMapping(L"Cube/Pyramid",
									L"Cube/Pyramid")), (gcnew System::Data::Common::DataColumnMapping(L"Color", L"Color"))
			};
			this->oleDbDataAdapter1->TableMappings->AddRange(gcnew cli::array< System::Data::Common::DataTableMapping^  >(1) {
				(gcnew System::Data::Common::DataTableMapping(L"Table",
					L"Shapes", __mcTemp__1))
			});
			this->oleDbDataAdapter1->UpdateCommand = this->oleDbUpdateCommand1;
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
			this->ControlTab->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataSet1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	// Functions and Events
	private: void InitializeGL()
	{
		FrameWidth = GLFrame->Width;
		FrameHeight = GLFrame->Height;
		FrameDepth = GLFrame->Height;
		GL::MatrixMode(MatrixMode::Projection);
		GL::LoadIdentity();
		GL::Ortho(-(FrameWidth >> 1), FrameWidth >> 1, -(FrameHeight >> 1), FrameHeight >> 1, -(FrameDepth >> 1), FrameDepth >> 1);
		GL::Viewport(0, 0, FrameWidth, FrameHeight);
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
		GL::MatrixMode(MatrixMode::Modelview);
		GL::LoadIdentity();

		float specular[] = { 1.0, 1.0, 1.0, 1.0 };
		float shininess = 50.0;
		GL::ClearColor(80 / 255.0, 80 / 255.0, 80 / 255.0, 0.1);
		GL::ShadeModel(ShadingModel::Smooth);
		GL::Material(MaterialFace::Front, MaterialParameter::Specular, specular);
		GL::Material(MaterialFace::Front, MaterialParameter::Shininess, shininess);
		GL::Enable(EnableCap::ColorMaterial);
		GL::Enable(EnableCap::Lighting);
		GL::Enable(EnableCap::Light0);
		GL::Enable(EnableCap::DepthTest);
	}
	private: void LoadData()
	{
		oleDbConnection1->Open();
		oleDbDataAdapter1->Fill(dataSet1);
		cnt_shapes = Convert::ToDouble(dataSet1->Tables[0]->Rows[dataSet1->Tables[0]->Rows->Count - 1]->ItemArray[0]) + 1;
		dataGridView->AutoGenerateColumns = true;
		dataGridView->DataSource = dataSet1->Tables[0];
		dataGridView->Update();
		oleDbConnection1->Close();
		dataGridView->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
		dataGridView->Columns[0]->Visible = false;
		for (int i = 0; i < dataGridView->RowCount; i++)
		{
			ShapeType type = ShapeType::Pyramid;
			bool t = Convert::ToBoolean(dataGridView->Rows[i]->Cells[8]->Value);
			if (t) type = ShapeType::Cube;

			Color color; int col = Convert::ToInt32(dataGridView->Rows[i]->Cells[9]->Value);
			switch (col)
			{
			case 0: { color = Color::Red; } break;
			case 1: { color = Color::Orange; } break;
			case 2: { color = Color::Yellow; } break;
			case 3: { color = Color::Green; } break;
			case 4: { color = Color::Aqua; } break;
			case 5: { color = Color::Blue; } break;
			case 6: { color = Color::Purple; } break;
			case 7: { color = Color::Brown; } break;
			default: { color = Color::Gray; } break;
			}
			engine.CreateShape(type, color,	Convert::ToDouble(dataGridView->Rows[i]->Cells[4]->Value), 
											Convert::ToDouble(dataGridView->Rows[i]->Cells[1]->Value), 
											Convert::ToDouble(dataGridView->Rows[i]->Cells[2]->Value), 
											Convert::ToDouble(dataGridView->Rows[i]->Cells[3]->Value), 
											Convert::ToDouble(dataGridView->Rows[i]->Cells[5]->Value),
											Convert::ToDouble(dataGridView->Rows[i]->Cells[6]->Value), 
											Convert::ToDouble(dataGridView->Rows[i]->Cells[7]->Value), 
											Convert::ToDouble(dataGridView->Rows[i]->Cells[0]->Value));
		}
	}

	private: void DrawAll()
	{
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
		DrawOrthPlanes();
		DrawOrthLines();
		engine.DrawShapes();
		GLFrame->SwapBuffers();
	}
	private: void DrawOrthLines()
	{
		GL::Begin(BeginMode::Lines);
		if (cb1_axisX->Checked)
		{
			GL::Color3(Color::Red);
			GL::Vertex3(0.0, 0.0, 0.0);
			GL::Vertex3(double(FrameWidth >> 1), 0.0, 0.0);
		}
		if (cb1_axisY->Checked)
		{
			GL::Color3(Color::Green);
			GL::Vertex3(0.0, 0.0, 0.0);
			GL::Vertex3(0.0, double(FrameHeight >> 1), 0.0);
		}
		if (cb1_axisZ->Checked)
		{
			GL::Color3(Color::Blue);
			GL::Vertex3(0.0, 0.0, 0.0);
			GL::Vertex3(0.0, 0.0, double(FrameDepth >> 1));
		}
		GL::End();
	}
	private: void DrawOrthPlanes()
	{
		GL::Begin(BeginMode::Lines);
		if (cb1_planeXY->Checked)
		{
			GL::Color3(Color::Red);
			for (double i = -FrameWidth >> 1; i <= FrameWidth >> 1; i += FrameWidth >> 4)
			{
				GL::Vertex3(i, (double)-FrameHeight, 0.0);
				GL::Vertex3(i, (double)FrameHeight, 0.0);
			}
			for (double i = -FrameHeight >> 1; i <= FrameHeight >> 1; i += FrameHeight >> 4)
			{
				GL::Vertex3((double)-FrameWidth, i, 0.0);
				GL::Vertex3((double)FrameWidth, i, 0.0);
			}
		}
		if (cb1_planeXZ->Checked)
		{
			GL::Color3(Color::Green);
			for (double i = -FrameWidth >> 1; i <= FrameWidth >> 1; i += FrameWidth >> 4)
			{
				GL::Vertex3(i, 0.0, (double)-FrameDepth);
				GL::Vertex3(i, 0.0, (double)FrameDepth);
			}
			for (double i = -FrameDepth >> 1; i <= FrameDepth >> 1; i += FrameDepth >> 4)
			{
				GL::Vertex3((double)-FrameWidth, 0.0, i);
				GL::Vertex3((double)FrameWidth, 0.0, i);
			}
		}
		if (cb1_planeYZ->Checked)
		{
			GL::Color3(Color::Blue);
			for (double i = -FrameHeight >> 1; i <= FrameHeight >> 1; i += FrameHeight >> 4)
			{
				GL::Vertex3(0.0, i, (double)-FrameDepth);
				GL::Vertex3(0.0, i, (double)FrameDepth);
			}
			for (double i = -FrameDepth >> 1; i <= FrameDepth >> 1; i += FrameDepth >> 4)
			{
				GL::Vertex3(0.0, (double)-FrameHeight, i);
				GL::Vertex3(0.0, (double)FrameHeight, i);
			}
		}
		GL::End();
	}

	// В дооолгий ящик
			 /*
	private: int SelectObject(double x, double y)
	{
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);

		int objectsFound = 0;
		int viewportCoords[4] = { 0 };
		unsigned int selectBuffer[400] = { 0 };

		GL::SelectBuffer(400, selectBuffer);
		GL::GetInteger(GetPName::Viewport, viewportCoords);
		GL::MatrixMode(MatrixMode::Projection);
		GL::PushMatrix();
		GL::RenderMode(RenderingMode::Select);
		GL::LoadIdentity();

		PickMatrix(x, viewportCoords[3] - y, 2, 2, viewportCoords);
		Perspective(45.0f, 742.0 / 536.0, 0.1f, 150.0f);

		GL::MatrixMode(MatrixMode::Modelview);
		DrawAll();
		objectsFound = GL::RenderMode(RenderingMode::Render);
		GL::MatrixMode(MatrixMode::Projection);
		GL::PopMatrix();
		GL::MatrixMode(MatrixMode::Modelview);

		if (objectsFound == 0) return 0;
		unsigned int lowestDepth = selectBuffer[1];
		int selectedObject = selectBuffer[3];
		for (int i = 1; i < objectsFound; i++)
		{
			if (selectBuffer[(i * 4) + 1] < lowestDepth)
			{
				lowestDepth = selectBuffer[(i * 4) + 1];
				selectedObject = selectBuffer[(i * 4) + 3];
			}
		}
		return selectedObject;
	}
	private: void PickMatrix(double x, double y, double deltax, double deltay, int* viewport)
	{
		if (deltax <= 0 || deltay <= 0) return;
		// Translate and scale the picked region to cover the canvas
		GL::Translate(
			(viewport[2] - 2 * (x - viewport[0])) / deltax, 
			(viewport[3] - 2 * (y - viewport[1])) / deltay, 
			0);
		GL::Scale(viewport[2] / deltax, viewport[3] / deltay, 1.0);
	}
	private: void Perspective(float a, float b, float c, float d)
	{
		OpenTK::Matrix4 projectionMatrix = OpenTK::Matrix4::CreatePerspectiveFieldOfView((float)((a*OpenTK::MathHelper::Pi)/180.0), b, c, d);
		GL::MatrixMode(MatrixMode::Projection);
		GL::LoadMatrix(projectionMatrix);
	}

	*/

	private: System::Void MyForm_Shown(System::Object^  sender, System::EventArgs^  e)
	{
		InitializeGL();
		LoadData();
		DrawAll();
	}
	private: System::Void GLFrame_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		engine.CameraControl(e);
		tb1_LightX->Text = Convert::ToString(engine.GetLightX());
		tb1_LightY->Text = Convert::ToString(engine.GetLightY());
		tb1_LightZ->Text = Convert::ToString(engine.GetLightZ());
		DrawAll();
	}
	private: System::Void GLFrame_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		if (e->Button == System::Windows::Forms::MouseButtons::Right) engine.DeselectAll();
		else if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			double x = 0; // (MousePosition.X - this->Left - GLFrame->Left - 8);// -(742 / 2.0);// / (742 / 2.0) - 1;
			double y = 0; // -(MousePosition.Y - this->Top - GLFrame->Top - 31) + 536;// +(536 / 2.0);// / (536 / 2.0) + 1;
						  //int selected = SelectObject(x, y);
						  //engine.SelectShape(selected);
		}
		DrawAll();
	}

	private: System::Void tb1_trans_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb1_trans->Text->Length == 0 || tb1_trans->Text == "-") tb1_trans->Text = "0";
		engine.SetTranslateStep(Convert::ToDouble(tb1_trans->Text));
	}
	private: System::Void tb1_rot_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb1_rot->Text->Length == 0 || tb1_rot->Text == "-") tb1_rot->Text = "0";
		engine.SetRotareStep(Convert::ToDouble(tb1_rot->Text));
	}
	private: System::Void tb1_scal_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb1_scal->Text->Length == 0 || tb1_scal->Text == "-") tb1_scal->Text = "0";
		engine.SetScaleStep(Double::Parse(tb1_scal->Text));
	}
	private: System::Void tb1_trans_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) engine.SetTranslateStep(Convert::ToDouble(tb1_trans->Text));
		else if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb1_trans->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb1_rot_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) engine.SetRotareStep(Convert::ToDouble(tb1_rot->Text));
		else if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb1_rot->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb1_scal_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) engine.SetScaleStep(Double::Parse(tb1_scal->Text));
		else if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8 && e->KeyChar != 44)
		{
			if (tb1_scal->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void cb1_axisX_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		DrawAll();
	}
	private: System::Void cb1_axisY_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		DrawAll();
	}
	private: System::Void cb1_axisZ_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		DrawAll();
	}
	private: System::Void cb1_planeXY_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		DrawAll();
	}
	private: System::Void cb1_planeXZ_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		DrawAll();
	}
	private: System::Void cb1_planeYZ_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		DrawAll();
	}
	private: System::Void but1LightNull_Click(System::Object^  sender, System::EventArgs^  e)
	{
		engine.NullLightPos();
		tb1_LightX->Text = "0";
		tb1_LightY->Text = "0";
		tb1_LightZ->Text = "0";
	}

	private: System::Void tb2_Xcoor_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb2_Xcoor->Text == "0") tb2_Xcoor->Text = "";
	}
	private: System::Void tb2_Ycoor_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb2_Ycoor->Text == "0") tb2_Ycoor->Text = "";
	}
	private: System::Void tb2_Zcoor_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb2_Zcoor->Text == "0") tb2_Zcoor->Text = "";
	}
	private: System::Void tb2_Xrot_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb2_Xrot->Text == "0") tb2_Xrot->Text = "";
	}
	private: System::Void tb2_Yrot_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb2_Yrot->Text == "0") tb2_Yrot->Text = "";
	}
	private: System::Void tb2_Zrot_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb2_Zrot->Text == "0") tb2_Zrot->Text = "";
	}
	private: System::Void tb2_leght_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb2_leght->Text->Length == 0 || tb2_leght->Text == "-") tb2_leght->Text = "50";
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
	private: System::Void but_AddShape_Click(System::Object^  sender, System::EventArgs^  e)
	{
		SystemSounds::Beep->Play();
		ShapeType type;
		bool b = true;
		if (ShapeListBox2->SelectedItem == ShapeListBox2->Items[0]) type = ShapeType::Cube;
		else if (ShapeListBox2->SelectedItem == ShapeListBox2->Items[1]) { type = ShapeType::Pyramid; b = false; }

		Color col; int k;
		if (ColorBox2->SelectedItem == ColorBox2->Items[0]) { col = Color::Red; k = 0; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[1]) { col = Color::Orange; k = 1; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[2]) { col = Color::Yellow; k = 2; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[3]) { col = Color::Green; k = 3; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[4]) { col = Color::Aqua; k = 4; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[5]) { col = Color::Blue; k = 5; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[6]) { col = Color::Purple; k = 6; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[7]) { col = Color::Brown; k = 7; }
		else if (ColorBox2->SelectedItem == ColorBox2->Items[8]) { col = Color::Gray; k = 8; }

		engine.CreateShape(type, col, Convert::ToUInt32(tb2_leght->Text),
			Convert::ToDouble(tb2_Xcoor->Text),
			Convert::ToDouble(tb2_Ycoor->Text),
			Convert::ToDouble(tb2_Zcoor->Text),
			Convert::ToDouble(tb2_Xrot->Text),
			Convert::ToDouble(tb2_Yrot->Text),
			Convert::ToDouble(tb2_Zrot->Text), cnt_shapes);

		dataSet1->Tables[0]->Rows->Add(cnt_shapes, tb2_Xcoor->Text, tb2_Ycoor->Text, tb2_Ycoor->Text, tb2_leght->Text, tb2_Xrot->Text, tb2_Xrot->Text, tb2_Yrot->Text, b, k);
		dataGridView->Update();
		oleDbDataAdapter1->Update(dataSet1);
		cnt_shapes++;
		DrawAll();
	}

	private: System::Void dataGridView_UserDeletingRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowCancelEventArgs^  e) 
	{
		int id = Convert::ToInt32(e->Row->Cells[0]->Value);
		engine.DeleteShape(id);
		DrawAll();
	}
	private: System::Void dataGridView_UserDeletedRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowEventArgs^  e) 
	{
		oleDbDataAdapter1->Update(dataSet1);
	}
};
}