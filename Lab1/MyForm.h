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
	size_t next_id = 1;
	int current_scene = 1;
	bool selected = false;
	int selected_shape_id = -1;

	OpenTK::Vector3 eyes(300.0f, 300.0f, 300.0f);
	OpenTK::Vector3 direction(0, 0, -1);
	OpenTK::Vector3 target(0, 0, 0);
	OpenTK::Vector3 up(0, 1, 0);

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
			size_t cnt;
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
			virtual void Draw() {}
			virtual void DrawProjections(bool xy, bool xz, bool yz) {}
			void Select()
			{
				this->selected = true;
			}
			void Deselect()
			{
				this->selected = false;
			}
			virtual double Intersect(OpenTK::Vector3 ray, OpenTK::Vector3 camera_pos) { return -1; }

			size_t GetID()
			{
				return this->id;
			}
			OpenTK::Vector3 GetPos()
			{
				return OpenTK::Vector3((float)x_coor, (float)y_coor, (float)z_coor);
			}
			OpenTK::Vector3 GetDirection()
			{
				return OpenTK::Vector3((float)x_rot, (float)y_rot, (float)z_rot);
			}
			size_t GetLength()
			{
				return l;
			}
			Color GetColor()
			{
				return color;
			}
		protected:
			double distance(OpenTK::Vector3 vect, OpenTK::Vector3 point, OpenTK::Vector3 f_point)
			{
				OpenTK::Vector3 pnt(point.X - f_point.X, point.Y - f_point.Y, point.Z - f_point.Z);
				OpenTK::Vector3 mlt((pnt.Y*vect.Z) - (pnt.Z*vect.Y), -((pnt.Z*vect.X) - (pnt.X*vect.Z)), (pnt.X*vect.Y) - (pnt.Y*vect.X));
				return Math::Sqrt((mlt.X*mlt.X + mlt.Y*mlt.Y + mlt.Z*mlt.Z) / (vect.X*vect.X + vect.Y*vect.Y + vect.Z*vect.Z));
			}
			double distance(OpenTK::Vector3 p1, OpenTK::Vector3 p2)
			{
				return Math::Sqrt((p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y) + (p1.Z - p2.Z)*(p1.Z - p2.Z));
			}
		};
		ref class Cube : public Shape
		{
			double *x, *y, *z;
		public:
			Cube()
			{
				cnt = 8;
				x = y = z = nullptr;
			}
			~Cube()
			{
				delete[] x, y, z;
			}
			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id) override
			{
				x_rot = OpenTK::MathHelper::DegreesToRadians(x_rot);
				y_rot = OpenTK::MathHelper::DegreesToRadians(y_rot);
				z_rot = OpenTK::MathHelper::DegreesToRadians(z_rot);
				Shape::SetData(color, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, id);
				x = new double[cnt];
				y = new double[cnt];
				z = new double[cnt];
				x[0] = x[1] = x[4] = x[5] =  -((int)l / 2.0);
				x[2] = x[3] = x[6] = x[7] =   ((int)l / 2.0);
				y[0] = y[1] = y[2] = y[3] =  -((int)l / 2.0);
				y[4] = y[5] = y[6] = y[7] =   ((int)l / 2.0);
				z[0] = z[3] = z[4] = z[7] =  -((int)l / 2.0);
				z[1] = z[2] = z[5] = z[6] =   ((int)l / 2.0);

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
				for (size_t i = 0; i < cnt; i++)
				{
					x[i] += x_coor;
					y[i] += y_coor;
					z[i] += z_coor;
				}
			}
			virtual void Draw() override
			{
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

				GL::Color3(Color::Black);
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
			}
			virtual void DrawProjections(bool xy, bool xz, bool yz) override
			{
				GL::Color3(color);
				if (xy)
				{
					GL::Begin(BeginMode::Quads);
					GL::Vertex3(x[0], y[0], 0.0);
					GL::Vertex3(x[1], y[1], 0.0);
					GL::Vertex3(x[2], y[2], 0.0);
					GL::Vertex3(x[3], y[3], 0.0);

					GL::Vertex3(x[2], y[2], 0.0);
					GL::Vertex3(x[3], y[3], 0.0);
					GL::Vertex3(x[7], y[7], 0.0);
					GL::Vertex3(x[6], y[6], 0.0);

					GL::Vertex3(x[7], y[7], 0.0);
					GL::Vertex3(x[6], y[6], 0.0);
					GL::Vertex3(x[5], y[5], 0.0);
					GL::Vertex3(x[4], y[4], 0.0);

					GL::Vertex3(x[5], y[5], 0.0);
					GL::Vertex3(x[4], y[4], 0.0);
					GL::Vertex3(x[0], y[0], 0.0);
					GL::Vertex3(x[1], y[1], 0.0);

					GL::Vertex3(x[1], y[1], 0.0);
					GL::Vertex3(x[2], y[2], 0.0);
					GL::Vertex3(x[6], y[6], 0.0);
					GL::Vertex3(x[5], y[5], 0.0);

					GL::Vertex3(x[0], y[0], 0.0);
					GL::Vertex3(x[3], y[3], 0.0);
					GL::Vertex3(x[7], y[7], 0.0);
					GL::Vertex3(x[4], y[4], 0.0);
					GL::End();
				}
				if (xz)
				{
					GL::Begin(BeginMode::Quads);
					GL::Vertex3(x[0], 0.0, z[0]);
					GL::Vertex3(x[1], 0.0, z[1]);
					GL::Vertex3(x[2], 0.0, z[2]);
					GL::Vertex3(x[3], 0.0, z[3]);

					GL::Vertex3(x[2], 0.0, z[2]);
					GL::Vertex3(x[3], 0.0, z[3]);
					GL::Vertex3(x[7], 0.0, z[7]);
					GL::Vertex3(x[6], 0.0, z[6]);

					GL::Vertex3(x[7], 0.0, z[7]);
					GL::Vertex3(x[6], 0.0, z[6]);
					GL::Vertex3(x[5], 0.0, z[5]);
					GL::Vertex3(x[4], 0.0, z[4]);

					GL::Vertex3(x[5], 0.0, z[5]);
					GL::Vertex3(x[4], 0.0, z[4]);
					GL::Vertex3(x[0], 0.0, z[0]);
					GL::Vertex3(x[1], 0.0, z[1]);

					GL::Vertex3(x[1], 0.0, z[1]);
					GL::Vertex3(x[2], 0.0, z[2]);
					GL::Vertex3(x[6], 0.0, z[6]);
					GL::Vertex3(x[5], 0.0, z[5]);

					GL::Vertex3(x[0], 0.0, z[0]);
					GL::Vertex3(x[3], 0.0, z[3]);
					GL::Vertex3(x[7], 0.0, z[7]);
					GL::Vertex3(x[4], 0.0, z[4]);
					GL::End();
				}
				if (yz)
				{
					GL::Begin(BeginMode::Quads);
					GL::Vertex3(0.0, y[0], z[0]);
					GL::Vertex3(0.0, y[1], z[1]);
					GL::Vertex3(0.0, y[2], z[2]);
					GL::Vertex3(0.0, y[3], z[3]);

					GL::Vertex3(0.0, y[2], z[2]);
					GL::Vertex3(0.0, y[3], z[3]);
					GL::Vertex3(0.0, y[7], z[7]);
					GL::Vertex3(0.0, y[6], z[6]);

					GL::Vertex3(0.0, y[7], z[7]);
					GL::Vertex3(0.0, y[6], z[6]);
					GL::Vertex3(0.0, y[5], z[5]);
					GL::Vertex3(0.0, y[4], z[4]);

					GL::Vertex3(0.0, y[5], z[5]);
					GL::Vertex3(0.0, y[4], z[4]);
					GL::Vertex3(0.0, y[0], z[0]);
					GL::Vertex3(0.0, y[1], z[1]);

					GL::Vertex3(0.0, y[1], z[1]);
					GL::Vertex3(0.0, y[2], z[2]);
					GL::Vertex3(0.0, y[6], z[6]);
					GL::Vertex3(0.0, y[5], z[5]);

					GL::Vertex3(0.0, y[0], z[0]);
					GL::Vertex3(0.0, y[3], z[3]);
					GL::Vertex3(0.0, y[7], z[7]);
					GL::Vertex3(0.0, y[4], z[4]);
					GL::End();
				}
			}
			virtual double Intersect(OpenTK::Vector3 ray, OpenTK::Vector3 camera_pos) override
			{
				for (size_t i = 0; i < cnt; i++)
				{
					double k = Shape::distance(ray, camera_pos, OpenTK::Vector3(float(x[i]), float(y[i]), float(z[i])));
					if (k > l*Math::Sqrt(3)) return -1;
				}
				return Shape::distance(OpenTK::Vector3(float(x_coor), float(y_coor), float(z_coor)), camera_pos);
			}
		};
		ref class Pyramid : public Shape
		{
			double *x, *y, *z;
		public:
			Pyramid()
			{
				cnt = 4;
				x = y = z = nullptr;
			}
			~Pyramid()
			{
				delete[] x, y, z;
			}

			virtual void SetData(Color color, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id) override
			{
				x_rot = OpenTK::MathHelper::DegreesToRadians(x_rot);
				y_rot = OpenTK::MathHelper::DegreesToRadians(y_rot);
				z_rot = OpenTK::MathHelper::DegreesToRadians(z_rot);
				Shape::SetData(color, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, id);
				x = new double[cnt];
				y = new double[cnt];
				z = new double[cnt];
				x[0] = x[2] = -((int)l / Math::Sqrt(12)); x[1] = ((int)l / Math::Sqrt(3)); x[3] = 0;
				y[0] = y[1] = y[2] = 0; y[3] = Math::Sqrt((2 * l*l) / 3);
				z[0] =  -((int)l / 2); z[1] = z[3] = 0; z[2] = (l / 2);

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
				for (size_t i = 0; i < cnt; i++)
				{
					x[i] += x_coor;
					y[i] += y_coor;
					z[i] += z_coor;
				}
			}
			virtual void Draw() override
			{
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

				GL::Color3(Color::Black);
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
			}
			virtual void DrawProjections(bool xy, bool xz, bool yz) override
			{
				GL::Color3(color);
				if (xy)
				{
					GL::Begin(BeginMode::Triangles);
					GL::Vertex3(x[0], y[0], 0.0);
					GL::Vertex3(x[1], y[1], 0.0);
					GL::Vertex3(x[2], y[2], 0.0);

					GL::Vertex3(x[0], y[0], 0.0);
					GL::Vertex3(x[3], y[3], 0.0);
					GL::Vertex3(x[2], y[2], 0.0);

					GL::Vertex3(x[0], y[0], 0.0);
					GL::Vertex3(x[3], y[3], 0.0);
					GL::Vertex3(x[1], y[1], 0.0);

					GL::Vertex3(x[2], y[2], 0.0);
					GL::Vertex3(x[3], y[3], 0.0);
					GL::Vertex3(x[1], y[1], 0.0);
					GL::End();
				}
				if (xz)
				{
					GL::Begin(BeginMode::Triangles);
					GL::Vertex3(x[0], 0.0, z[0]);
					GL::Vertex3(x[1], 0.0, z[1]);
					GL::Vertex3(x[2], 0.0, z[2]);

					GL::Vertex3(x[0], 0.0, z[0]);
					GL::Vertex3(x[3], 0.0, z[3]);
					GL::Vertex3(x[2], 0.0, z[2]);

					GL::Vertex3(x[0], 0.0, z[0]);
					GL::Vertex3(x[3], 0.0, z[3]);
					GL::Vertex3(x[1], 0.0, z[1]);

					GL::Vertex3(x[2], 0.0, z[2]);
					GL::Vertex3(x[3], 0.0, z[3]);
					GL::Vertex3(x[1], 0.0, z[1]);
					GL::End();
				}
				if (yz)
				{
					GL::Begin(BeginMode::Triangles);
					GL::Vertex3(0.0, y[0], z[0]);
					GL::Vertex3(0.0, y[1], z[1]);
					GL::Vertex3(0.0, y[2], z[2]);

					GL::Vertex3(0.0, y[0], z[0]);
					GL::Vertex3(0.0, y[3], z[3]);
					GL::Vertex3(0.0, y[2], z[2]);

					GL::Vertex3(0.0, y[0], z[0]);
					GL::Vertex3(0.0, y[3], z[3]);
					GL::Vertex3(0.0, y[1], z[1]);

					GL::Vertex3(0.0, y[2], z[2]);
					GL::Vertex3(0.0, y[3], z[3]);
					GL::Vertex3(0.0, y[1], z[1]);
					GL::End();
				}
			}
			virtual double Intersect(OpenTK::Vector3 ray, OpenTK::Vector3 camera_pos) override
			{
				for (size_t i = 0; i < cnt; i++)
				{
					if (Shape::distance(ray, camera_pos, OpenTK::Vector3((float)x[i], (float)y[i], (float)z[i])) > (l*5)/6) return -1;
				}
				return Shape::distance(OpenTK::Vector3((float)x_coor, (float)y_coor, (float)z_coor), camera_pos);
			}
		};

		ref class Engine
		{
			List<Shape^> shape_list;
			double TranslateStep;
			double ScaleStep;
			double RotareStep;
			OpenTK::Vector3 light_pos;
		public:
			Engine()
			{
				TranslateStep = 10;
				ScaleStep = 10;
				RotareStep = 5;
				light_pos = OpenTK::Vector3(100, 100, 100);
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
			OpenTK::Vector3 GetLight()
			{
				return light_pos;
			}
			void CameraControl(System::Windows::Forms::KeyPressEventArgs^  &e)
			{
				switch (e->KeyChar)
				{
				case 'w':
				{
					eyes += up * (float)TranslateStep;
					target += up * (float)TranslateStep;
				} break;
				case 's':
				{
					eyes += (-up) * (float)TranslateStep;
					target += (-up) * (float)TranslateStep;
				} break;
				case 'e':
				{
					eyes += direction * (float)ScaleStep;
					target += direction * (float)ScaleStep;
				} break;
				case 'q':
				{
					eyes += (-direction) * (float)ScaleStep;
					target += (-direction) * (float)ScaleStep;
				} break;
				case 'a':
				{
					OpenTK::Vector3 new_dir((float)(up.Y*direction.Z - up.Z*direction.Y), (float)(-(up.Z*direction.Y - up.X*direction.Z)), (float)(up.X*direction.Y - up.Y*direction.X));
					eyes += new_dir * (float)TranslateStep;
					target += new_dir * (float)TranslateStep;
				} break;
				case 'd':
				{
					OpenTK::Vector3 new_dir((float)(up.Y*direction.Z - up.Z*direction.Y), (float)(-(up.Z*direction.Y - up.X*direction.Z)), (float)(up.X*direction.Y - up.Y*direction.X));
					eyes += (-new_dir) * (float)TranslateStep;
					target += (-new_dir) * (float)TranslateStep;
				} break;

				case 't':
				{
					direction += (up / 100.0f) * (float)RotareStep;
					direction.Normalize();
				} break;
				case 'g':
				{
					direction += (-up / 100.0f) * (float)RotareStep;
					direction.Normalize();
				} break;
				case 'f':
				{
					OpenTK::Vector3 new_dir((float)(up.Y*direction.Z - up.Z*direction.Y), (float)(-(up.Z*direction.Y - up.X*direction.Z)), (float)(up.X*direction.Y - up.Y*direction.X));
					direction += (new_dir / 100.0f) * (float)RotareStep;
					direction.Normalize();
				} break;
				case 'h':
				{
					OpenTK::Vector3 new_dir((float)(up.Y*direction.Z - up.Z*direction.Y), (float)(-(up.Z*direction.Y - up.X*direction.Z)), (float)(up.X*direction.Y - up.Y*direction.X));
					direction += (-new_dir / 100.0f) * (float)RotareStep;
					direction.Normalize();
				} break;

				case 'u': { light_pos[0] += (float)TranslateStep; } break;
				case 'j': { light_pos[0] -= (float)TranslateStep; } break;
				case 'i': { light_pos[1] += (float)TranslateStep; } break;
				case 'k': { light_pos[1] -= (float)TranslateStep; } break;
				case 'o': { light_pos[2] += (float)TranslateStep; } break;
				case 'l': { light_pos[2] -= (float)TranslateStep; } break;
				}
			}

			void CreateShape(ShapeType type, Color col, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id)
			{
				Shape^ s;
				if (type == ShapeType::Cube) s = gcnew Cube();
				else s = gcnew Pyramid();
				s->SetData(col, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, id);
				this->shape_list.Add(s);
			}
			void ResetShape(ShapeType type, Color col, size_t l, double x_coor, double y_coor, double z_coor, double x_rot, double y_rot, double z_rot, size_t id)
			{
				int i = 0;
				for (; i < shape_list.Count; i++)
					if (shape_list[i]->GetID() == id) break;
				shape_list[i]->SetData(col, l, x_coor, y_coor, z_coor, x_rot, y_rot, z_rot, id);
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
			void RemoveShapes()
			{
				shape_list.Clear();
			}
			void DrawShapes()
			{
				for each (Shape^ cur in shape_list)
				{
					cur->Draw();
				}
				float l[4] = { light_pos.X, light_pos.Y, light_pos.Z, 1 };
				GL::Light(LightName::Light0, LightParameter::Position, l);
			}
			void DrawProjections(bool xy, bool xz, bool yz)
			{
				for each (Shape^ cur in shape_list)
				{
					cur->DrawProjections(xy, xz, yz);
				}
			}

			void SelectShape(size_t id)
			{
				selected = false;
				for (int i = 0; i < shape_list.Count; i++)
					if (shape_list[i]->GetID() == id)
					{
						shape_list[i]->Select();
						selected = true;
						return;
					}
			}
			void DeselectAll()
			{
				selected = false;
				for (int i = 0; i < shape_list.Count; i++)
					shape_list[i]->Deselect();
			}

			int Intersection(OpenTK::Vector3 ray, OpenTK::Vector3 camera_pos)
			{
				int id = -1; double tmp = -1;
				for each (Shape^ cur in shape_list)
				{
					double k = cur->Intersect(ray, camera_pos);
					if (k != -1 && (k < tmp || tmp == -1)) { tmp = k; id = int(cur->GetID()); }
				}
				return id;
			}
			Shape^ GetShape(size_t id)
			{
				int i = 0;
				for (; i < shape_list.Count; i++)
					if (shape_list[i]->GetID() == id) break;
				if (i == shape_list.Count) throw "Out of range";
				return shape_list[i];
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
	private: System::ComponentModel::IContainer^  components;
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
	private: System::Windows::Forms::TabControl^  MainTabControl;
	private: System::Windows::Forms::TabPage^  CameraTab;
	private: System::Windows::Forms::TabPage^  AddShapeTab;
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
	private: System::Windows::Forms::TextBox^  tb1_LightZ;
	private: System::Windows::Forms::TextBox^  tb1_LightY;
	private: System::Windows::Forms::TextBox^  tb1_LightX;
	private: System::Windows::Forms::Label^  label1LightZ;
	private: System::Windows::Forms::Label^  label1LightY;
	private: System::Windows::Forms::Label^  label1LightX;
	private: System::Windows::Forms::Label^  label1LightPos;
	private: System::Data::DataSet^  dataSet1;
	private: System::Windows::Forms::ColorDialog^  colorDialog;
	private: System::Windows::Forms::Button^  b2_setColor;
	private: System::Windows::Forms::PictureBox^  picture2;
	private: System::Windows::Forms::TextBox^  tb1_Scene;
	private: System::Windows::Forms::Label^  label1Scene;
	private: System::Windows::Forms::Panel^  panel3ControlUI;
	private: System::Windows::Forms::Label^  label3None;
	private: System::Windows::Forms::Label^  label3Selected;
	private: System::Windows::Forms::ComboBox^  ShapeTypeBox3;
	private: System::Windows::Forms::Label^  label3ShapeType;
	private: System::Windows::Forms::Label^  label3ZPos;
	private: System::Windows::Forms::TextBox^  tb3_ZPos;
	private: System::Windows::Forms::TextBox^  tb3_YPos;
	private: System::Windows::Forms::Label^  label3YPos;
	private: System::Windows::Forms::Label^  label3XPos;
	private: System::Windows::Forms::TextBox^  tb3_XPos;
	private: System::Windows::Forms::Label^  label3Position;
	private: System::Windows::Forms::Button^  b3_PosXdown;
	private: System::Windows::Forms::Button^  b3_PosZdown;
	private: System::Windows::Forms::Button^  b3_PosZup;
	private: System::Windows::Forms::Button^  b3_PosYdown;
	private: System::Windows::Forms::Button^  b3_PosXup;
	private: System::Windows::Forms::Button^  b3_PosYup;
	private: System::Windows::Forms::Button^  b3_RotXdown;
	private: System::Windows::Forms::Button^  b3_RotZdown;
	private: System::Windows::Forms::Button^  b3_RotZup;
	private: System::Windows::Forms::Button^  b3_RotYdown;
	private: System::Windows::Forms::Button^  b3_RotXup;
	private: System::Windows::Forms::Button^  b3_RotYup;
	private: System::Windows::Forms::Label^  label3ZRot;
	private: System::Windows::Forms::TextBox^  tb3_ZRot;
	private: System::Windows::Forms::TextBox^  tb3_YRot;
	private: System::Windows::Forms::Label^  label3YRot;
	private: System::Windows::Forms::Label^  label3XRot;
	private: System::Windows::Forms::TextBox^  tb3_XRot;
	private: System::Windows::Forms::Label^  label3Rotare;
	private: System::Windows::Forms::Label^  label3Length;
	private: System::Windows::Forms::Button^  b3_LengthUp;
	private: System::Windows::Forms::Button^  b3_LengthDown;
	private: System::Windows::Forms::TextBox^  tb3_Length;
	private: System::Windows::Forms::TextBox^  tb3_Step;
	private: System::Windows::Forms::Label^  label3Step;
	private: System::Windows::Forms::Button^  b3_Color;
	private: System::Windows::Forms::PictureBox^  picture3_Color;
	private: System::Windows::Forms::ColorDialog^  colorDialog3;
	private: System::Windows::Forms::Button^  b3_deleteShape;
	private: System::Data::OleDb::OleDbCommand^  oleDbSelectCommand1;
	private: System::Data::OleDb::OleDbConnection^  oleDbConnection1;
	private: System::Data::OleDb::OleDbCommand^  oleDbInsertCommand1;
	private: System::Data::OleDb::OleDbCommand^  oleDbUpdateCommand1;
	private: System::Data::OleDb::OleDbCommand^  oleDbDeleteCommand1;
	private: System::Data::OleDb::OleDbDataAdapter^  oleDbDataAdapter1;

	private: Engine engine;
	private: Shape ^ current_shape;
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
			this->tb1_Scene = (gcnew System::Windows::Forms::TextBox());
			this->label1Scene = (gcnew System::Windows::Forms::Label());
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
			this->picture2 = (gcnew System::Windows::Forms::PictureBox());
			this->b2_setColor = (gcnew System::Windows::Forms::Button());
			this->ControlTab = (gcnew System::Windows::Forms::TabPage());
			this->panel3ControlUI = (gcnew System::Windows::Forms::Panel());
			this->b3_deleteShape = (gcnew System::Windows::Forms::Button());
			this->picture3_Color = (gcnew System::Windows::Forms::PictureBox());
			this->b3_Color = (gcnew System::Windows::Forms::Button());
			this->tb3_Step = (gcnew System::Windows::Forms::TextBox());
			this->label3Step = (gcnew System::Windows::Forms::Label());
			this->b3_LengthUp = (gcnew System::Windows::Forms::Button());
			this->b3_LengthDown = (gcnew System::Windows::Forms::Button());
			this->tb3_Length = (gcnew System::Windows::Forms::TextBox());
			this->label3Length = (gcnew System::Windows::Forms::Label());
			this->b3_RotXdown = (gcnew System::Windows::Forms::Button());
			this->b3_RotZdown = (gcnew System::Windows::Forms::Button());
			this->b3_RotZup = (gcnew System::Windows::Forms::Button());
			this->b3_RotYdown = (gcnew System::Windows::Forms::Button());
			this->b3_RotXup = (gcnew System::Windows::Forms::Button());
			this->b3_RotYup = (gcnew System::Windows::Forms::Button());
			this->label3ZRot = (gcnew System::Windows::Forms::Label());
			this->tb3_ZRot = (gcnew System::Windows::Forms::TextBox());
			this->tb3_YRot = (gcnew System::Windows::Forms::TextBox());
			this->label3YRot = (gcnew System::Windows::Forms::Label());
			this->label3XRot = (gcnew System::Windows::Forms::Label());
			this->tb3_XRot = (gcnew System::Windows::Forms::TextBox());
			this->label3Rotare = (gcnew System::Windows::Forms::Label());
			this->b3_PosXdown = (gcnew System::Windows::Forms::Button());
			this->b3_PosZdown = (gcnew System::Windows::Forms::Button());
			this->b3_PosZup = (gcnew System::Windows::Forms::Button());
			this->b3_PosYdown = (gcnew System::Windows::Forms::Button());
			this->b3_PosXup = (gcnew System::Windows::Forms::Button());
			this->b3_PosYup = (gcnew System::Windows::Forms::Button());
			this->label3ZPos = (gcnew System::Windows::Forms::Label());
			this->tb3_ZPos = (gcnew System::Windows::Forms::TextBox());
			this->tb3_YPos = (gcnew System::Windows::Forms::TextBox());
			this->label3YPos = (gcnew System::Windows::Forms::Label());
			this->label3XPos = (gcnew System::Windows::Forms::Label());
			this->tb3_XPos = (gcnew System::Windows::Forms::TextBox());
			this->label3Position = (gcnew System::Windows::Forms::Label());
			this->ShapeTypeBox3 = (gcnew System::Windows::Forms::ComboBox());
			this->label3ShapeType = (gcnew System::Windows::Forms::Label());
			this->label3None = (gcnew System::Windows::Forms::Label());
			this->label3Selected = (gcnew System::Windows::Forms::Label());
			this->dataSet1 = (gcnew System::Data::DataSet());
			this->colorDialog = (gcnew System::Windows::Forms::ColorDialog());
			this->colorDialog3 = (gcnew System::Windows::Forms::ColorDialog());
			this->oleDbSelectCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbInsertCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbUpdateCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbDeleteCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbDataAdapter1 = (gcnew System::Data::OleDb::OleDbDataAdapter());
			this->oleDbConnection1 = (gcnew System::Data::OleDb::OleDbConnection());
			this->MainTabControl->SuspendLayout();
			this->CameraTab->SuspendLayout();
			this->AddShapeTab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picture2))->BeginInit();
			this->ControlTab->SuspendLayout();
			this->panel3ControlUI->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picture3_Color))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataSet1))->BeginInit();
			this->SuspendLayout();
			// 
			// GLFrame
			// 
			this->GLFrame->BackColor = System::Drawing::Color::Black;
			this->GLFrame->Cursor = System::Windows::Forms::Cursors::NoMove2D;
			this->GLFrame->Location = System::Drawing::Point(9, 178);
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
			this->label2EdgeLenght->Location = System::Drawing::Point(11, 62);
			this->label2EdgeLenght->Name = L"label2EdgeLenght";
			this->label2EdgeLenght->Size = System::Drawing::Size(88, 17);
			this->label2EdgeLenght->TabIndex = 3;
			this->label2EdgeLenght->Text = L"Edge lenght:";
			// 
			// tb2_leght
			// 
			this->tb2_leght->Location = System::Drawing::Point(101, 59);
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
			this->MainTabControl->Location = System::Drawing::Point(12, 6);
			this->MainTabControl->Name = L"MainTabControl";
			this->MainTabControl->SelectedIndex = 0;
			this->MainTabControl->Size = System::Drawing::Size(995, 165);
			this->MainTabControl->TabIndex = 21;
			// 
			// CameraTab
			// 
			this->CameraTab->Controls->Add(this->tb1_Scene);
			this->CameraTab->Controls->Add(this->label1Scene);
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
			this->CameraTab->Size = System::Drawing::Size(987, 136);
			this->CameraTab->TabIndex = 0;
			this->CameraTab->Text = L"Camera";
			this->CameraTab->UseVisualStyleBackColor = true;
			// 
			// tb1_Scene
			// 
			this->tb1_Scene->Location = System::Drawing::Point(752, 31);
			this->tb1_Scene->Name = L"tb1_Scene";
			this->tb1_Scene->Size = System::Drawing::Size(100, 22);
			this->tb1_Scene->TabIndex = 25;
			this->tb1_Scene->Text = L"1";
			this->tb1_Scene->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb1_Scene_KeyPress);
			this->tb1_Scene->Leave += gcnew System::EventHandler(this, &MyForm::tb1_Scene_Leave);
			// 
			// label1Scene
			// 
			this->label1Scene->AutoSize = true;
			this->label1Scene->Location = System::Drawing::Point(749, 11);
			this->label1Scene->Name = L"label1Scene";
			this->label1Scene->Size = System::Drawing::Size(102, 17);
			this->label1Scene->TabIndex = 24;
			this->label1Scene->Text = L"Choose scene:";
			// 
			// tb1_LightZ
			// 
			this->tb1_LightZ->Location = System::Drawing::Point(662, 86);
			this->tb1_LightZ->Name = L"tb1_LightZ";
			this->tb1_LightZ->ReadOnly = true;
			this->tb1_LightZ->Size = System::Drawing::Size(59, 22);
			this->tb1_LightZ->TabIndex = 23;
			this->tb1_LightZ->Text = L"100";
			// 
			// tb1_LightY
			// 
			this->tb1_LightY->Location = System::Drawing::Point(662, 58);
			this->tb1_LightY->Name = L"tb1_LightY";
			this->tb1_LightY->ReadOnly = true;
			this->tb1_LightY->Size = System::Drawing::Size(59, 22);
			this->tb1_LightY->TabIndex = 22;
			this->tb1_LightY->Text = L"100";
			// 
			// tb1_LightX
			// 
			this->tb1_LightX->Location = System::Drawing::Point(662, 30);
			this->tb1_LightX->Name = L"tb1_LightX";
			this->tb1_LightX->ReadOnly = true;
			this->tb1_LightX->Size = System::Drawing::Size(59, 22);
			this->tb1_LightX->TabIndex = 21;
			this->tb1_LightX->Text = L"100";
			// 
			// label1LightZ
			// 
			this->label1LightZ->AutoSize = true;
			this->label1LightZ->Location = System::Drawing::Point(640, 89);
			this->label1LightZ->Name = L"label1LightZ";
			this->label1LightZ->Size = System::Drawing::Size(25, 17);
			this->label1LightZ->TabIndex = 20;
			this->label1LightZ->Text = L"Z :";
			// 
			// label1LightY
			// 
			this->label1LightY->AutoSize = true;
			this->label1LightY->Location = System::Drawing::Point(639, 63);
			this->label1LightY->Name = L"label1LightY";
			this->label1LightY->Size = System::Drawing::Size(25, 17);
			this->label1LightY->TabIndex = 19;
			this->label1LightY->Text = L"Y :";
			// 
			// label1LightX
			// 
			this->label1LightX->AutoSize = true;
			this->label1LightX->Location = System::Drawing::Point(640, 35);
			this->label1LightX->Name = L"label1LightX";
			this->label1LightX->Size = System::Drawing::Size(25, 17);
			this->label1LightX->TabIndex = 18;
			this->label1LightX->Text = L"X :";
			// 
			// label1LightPos
			// 
			this->label1LightPos->AutoSize = true;
			this->label1LightPos->Location = System::Drawing::Point(625, 11);
			this->label1LightPos->Name = L"label1LightPos";
			this->label1LightPos->Size = System::Drawing::Size(96, 17);
			this->label1LightPos->TabIndex = 17;
			this->label1LightPos->Text = L"Light position:";
			// 
			// rtb1_manual
			// 
			this->rtb1_manual->Location = System::Drawing::Point(357, 27);
			this->rtb1_manual->Name = L"rtb1_manual";
			this->rtb1_manual->ReadOnly = true;
			this->rtb1_manual->Size = System::Drawing::Size(262, 80);
			this->rtb1_manual->TabIndex = 16;
			this->rtb1_manual->Text = L"Translation:           Rotare:          Scale:\n  A-D/W-S               F-H/T-G   "
				L"     Q-E\n\nWarning! Use English keyboard layout.";
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
			this->tb1_scal->Text = L"10";
			this->tb1_scal->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb1_scal_KeyPress);
			this->tb1_scal->Leave += gcnew System::EventHandler(this, &MyForm::tb1_scal_Leave);
			// 
			// tb1_rot
			// 
			this->tb1_rot->Location = System::Drawing::Point(115, 41);
			this->tb1_rot->Name = L"tb1_rot";
			this->tb1_rot->Size = System::Drawing::Size(100, 22);
			this->tb1_rot->TabIndex = 4;
			this->tb1_rot->Text = L"5";
			this->tb1_rot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb1_rot_KeyPress);
			this->tb1_rot->Leave += gcnew System::EventHandler(this, &MyForm::tb1_rot_Leave);
			// 
			// tb1_trans
			// 
			this->tb1_trans->Location = System::Drawing::Point(115, 12);
			this->tb1_trans->Name = L"tb1_trans";
			this->tb1_trans->Size = System::Drawing::Size(100, 22);
			this->tb1_trans->TabIndex = 3;
			this->tb1_trans->Text = L"10";
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
			this->AddShapeTab->Controls->Add(this->picture2);
			this->AddShapeTab->Controls->Add(this->b2_setColor);
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
			this->AddShapeTab->Size = System::Drawing::Size(987, 136);
			this->AddShapeTab->TabIndex = 1;
			this->AddShapeTab->Text = L"Add shape";
			this->AddShapeTab->UseVisualStyleBackColor = true;
			// 
			// picture2
			// 
			this->picture2->Location = System::Drawing::Point(178, 89);
			this->picture2->Name = L"picture2";
			this->picture2->Size = System::Drawing::Size(31, 30);
			this->picture2->TabIndex = 22;
			this->picture2->TabStop = false;
			// 
			// b2_setColor
			// 
			this->b2_setColor->Location = System::Drawing::Point(101, 89);
			this->b2_setColor->Name = L"b2_setColor";
			this->b2_setColor->Size = System::Drawing::Size(70, 32);
			this->b2_setColor->TabIndex = 21;
			this->b2_setColor->Text = L"Color";
			this->b2_setColor->UseVisualStyleBackColor = true;
			this->b2_setColor->Click += gcnew System::EventHandler(this, &MyForm::b2_setColor_Click);
			// 
			// ControlTab
			// 
			this->ControlTab->Controls->Add(this->panel3ControlUI);
			this->ControlTab->Controls->Add(this->label3None);
			this->ControlTab->Controls->Add(this->label3Selected);
			this->ControlTab->Location = System::Drawing::Point(4, 25);
			this->ControlTab->Name = L"ControlTab";
			this->ControlTab->Size = System::Drawing::Size(987, 136);
			this->ControlTab->TabIndex = 2;
			this->ControlTab->Text = L"Control";
			this->ControlTab->UseVisualStyleBackColor = true;
			// 
			// panel3ControlUI
			// 
			this->panel3ControlUI->Controls->Add(this->b3_deleteShape);
			this->panel3ControlUI->Controls->Add(this->picture3_Color);
			this->panel3ControlUI->Controls->Add(this->b3_Color);
			this->panel3ControlUI->Controls->Add(this->tb3_Step);
			this->panel3ControlUI->Controls->Add(this->label3Step);
			this->panel3ControlUI->Controls->Add(this->b3_LengthUp);
			this->panel3ControlUI->Controls->Add(this->b3_LengthDown);
			this->panel3ControlUI->Controls->Add(this->tb3_Length);
			this->panel3ControlUI->Controls->Add(this->label3Length);
			this->panel3ControlUI->Controls->Add(this->b3_RotXdown);
			this->panel3ControlUI->Controls->Add(this->b3_RotZdown);
			this->panel3ControlUI->Controls->Add(this->b3_RotZup);
			this->panel3ControlUI->Controls->Add(this->b3_RotYdown);
			this->panel3ControlUI->Controls->Add(this->b3_RotXup);
			this->panel3ControlUI->Controls->Add(this->b3_RotYup);
			this->panel3ControlUI->Controls->Add(this->label3ZRot);
			this->panel3ControlUI->Controls->Add(this->tb3_ZRot);
			this->panel3ControlUI->Controls->Add(this->tb3_YRot);
			this->panel3ControlUI->Controls->Add(this->label3YRot);
			this->panel3ControlUI->Controls->Add(this->label3XRot);
			this->panel3ControlUI->Controls->Add(this->tb3_XRot);
			this->panel3ControlUI->Controls->Add(this->label3Rotare);
			this->panel3ControlUI->Controls->Add(this->b3_PosXdown);
			this->panel3ControlUI->Controls->Add(this->b3_PosZdown);
			this->panel3ControlUI->Controls->Add(this->b3_PosZup);
			this->panel3ControlUI->Controls->Add(this->b3_PosYdown);
			this->panel3ControlUI->Controls->Add(this->b3_PosXup);
			this->panel3ControlUI->Controls->Add(this->b3_PosYup);
			this->panel3ControlUI->Controls->Add(this->label3ZPos);
			this->panel3ControlUI->Controls->Add(this->tb3_ZPos);
			this->panel3ControlUI->Controls->Add(this->tb3_YPos);
			this->panel3ControlUI->Controls->Add(this->label3YPos);
			this->panel3ControlUI->Controls->Add(this->label3XPos);
			this->panel3ControlUI->Controls->Add(this->tb3_XPos);
			this->panel3ControlUI->Controls->Add(this->label3Position);
			this->panel3ControlUI->Controls->Add(this->ShapeTypeBox3);
			this->panel3ControlUI->Controls->Add(this->label3ShapeType);
			this->panel3ControlUI->Enabled = false;
			this->panel3ControlUI->Location = System::Drawing::Point(3, 24);
			this->panel3ControlUI->Name = L"panel3ControlUI";
			this->panel3ControlUI->Size = System::Drawing::Size(980, 109);
			this->panel3ControlUI->TabIndex = 5;
			// 
			// b3_deleteShape
			// 
			this->b3_deleteShape->Location = System::Drawing::Point(666, 14);
			this->b3_deleteShape->Name = L"b3_deleteShape";
			this->b3_deleteShape->Size = System::Drawing::Size(123, 86);
			this->b3_deleteShape->TabIndex = 35;
			this->b3_deleteShape->Text = L"Delete Shape";
			this->b3_deleteShape->UseVisualStyleBackColor = true;
			this->b3_deleteShape->Click += gcnew System::EventHandler(this, &MyForm::b3_deleteShape_Click);
			// 
			// picture3_Color
			// 
			this->picture3_Color->Location = System::Drawing::Point(90, 54);
			this->picture3_Color->Name = L"picture3_Color";
			this->picture3_Color->Size = System::Drawing::Size(30, 25);
			this->picture3_Color->TabIndex = 34;
			this->picture3_Color->TabStop = false;
			// 
			// b3_Color
			// 
			this->b3_Color->Location = System::Drawing::Point(7, 53);
			this->b3_Color->Name = L"b3_Color";
			this->b3_Color->Size = System::Drawing::Size(81, 26);
			this->b3_Color->TabIndex = 33;
			this->b3_Color->Text = L"Set color";
			this->b3_Color->UseVisualStyleBackColor = true;
			this->b3_Color->Click += gcnew System::EventHandler(this, &MyForm::b3_Color_Click);
			// 
			// tb3_Step
			// 
			this->tb3_Step->Location = System::Drawing::Point(48, 82);
			this->tb3_Step->Name = L"tb3_Step";
			this->tb3_Step->Size = System::Drawing::Size(72, 22);
			this->tb3_Step->TabIndex = 22;
			this->tb3_Step->Text = L"5";
			this->tb3_Step->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_Step_KeyPress);
			this->tb3_Step->Leave += gcnew System::EventHandler(this, &MyForm::tb3_Step_Leave);
			// 
			// label3Step
			// 
			this->label3Step->AutoSize = true;
			this->label3Step->Location = System::Drawing::Point(4, 83);
			this->label3Step->Name = L"label3Step";
			this->label3Step->Size = System::Drawing::Size(41, 17);
			this->label3Step->TabIndex = 32;
			this->label3Step->Text = L"Step:";
			// 
			// b3_LengthUp
			// 
			this->b3_LengthUp->Location = System::Drawing::Point(597, 56);
			this->b3_LengthUp->Name = L"b3_LengthUp";
			this->b3_LengthUp->Size = System::Drawing::Size(40, 30);
			this->b3_LengthUp->TabIndex = 31;
			this->b3_LengthUp->Text = L"▶";
			this->b3_LengthUp->UseVisualStyleBackColor = true;
			this->b3_LengthUp->Click += gcnew System::EventHandler(this, &MyForm::b3_LengthUp_Click);
			// 
			// b3_LengthDown
			// 
			this->b3_LengthDown->Location = System::Drawing::Point(551, 56);
			this->b3_LengthDown->Name = L"b3_LengthDown";
			this->b3_LengthDown->Size = System::Drawing::Size(40, 30);
			this->b3_LengthDown->TabIndex = 30;
			this->b3_LengthDown->Text = L"◀";
			this->b3_LengthDown->UseVisualStyleBackColor = true;
			this->b3_LengthDown->Click += gcnew System::EventHandler(this, &MyForm::b3_LengthDown_Click);
			// 
			// tb3_Length
			// 
			this->tb3_Length->Location = System::Drawing::Point(551, 30);
			this->tb3_Length->Name = L"tb3_Length";
			this->tb3_Length->Size = System::Drawing::Size(82, 22);
			this->tb3_Length->TabIndex = 29;
			this->tb3_Length->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_Length_KeyPress);
			this->tb3_Length->Leave += gcnew System::EventHandler(this, &MyForm::tb3_Length_Leave);
			// 
			// label3Length
			// 
			this->label3Length->AutoSize = true;
			this->label3Length->Location = System::Drawing::Point(548, 7);
			this->label3Length->Name = L"label3Length";
			this->label3Length->Size = System::Drawing::Size(56, 17);
			this->label3Length->TabIndex = 28;
			this->label3Length->Text = L"Length:";
			// 
			// b3_RotXdown
			// 
			this->b3_RotXdown->Location = System::Drawing::Point(452, 47);
			this->b3_RotXdown->Name = L"b3_RotXdown";
			this->b3_RotXdown->Size = System::Drawing::Size(30, 30);
			this->b3_RotXdown->TabIndex = 27;
			this->b3_RotXdown->Text = L"←";
			this->b3_RotXdown->UseVisualStyleBackColor = true;
			this->b3_RotXdown->Click += gcnew System::EventHandler(this, &MyForm::b3_RotXdown_Click);
			// 
			// b3_RotZdown
			// 
			this->b3_RotZdown->Location = System::Drawing::Point(452, 77);
			this->b3_RotZdown->Name = L"b3_RotZdown";
			this->b3_RotZdown->Size = System::Drawing::Size(30, 30);
			this->b3_RotZdown->TabIndex = 26;
			this->b3_RotZdown->Text = L"↙";
			this->b3_RotZdown->UseVisualStyleBackColor = true;
			this->b3_RotZdown->Click += gcnew System::EventHandler(this, &MyForm::b3_RotZdown_Click);
			// 
			// b3_RotZup
			// 
			this->b3_RotZup->Location = System::Drawing::Point(512, 17);
			this->b3_RotZup->Name = L"b3_RotZup";
			this->b3_RotZup->Size = System::Drawing::Size(30, 30);
			this->b3_RotZup->TabIndex = 25;
			this->b3_RotZup->Text = L"↗";
			this->b3_RotZup->UseVisualStyleBackColor = true;
			this->b3_RotZup->Click += gcnew System::EventHandler(this, &MyForm::b3_RotZup_Click);
			// 
			// b3_RotYdown
			// 
			this->b3_RotYdown->Location = System::Drawing::Point(482, 77);
			this->b3_RotYdown->Name = L"b3_RotYdown";
			this->b3_RotYdown->Size = System::Drawing::Size(30, 30);
			this->b3_RotYdown->TabIndex = 24;
			this->b3_RotYdown->Text = L"↓";
			this->b3_RotYdown->UseVisualStyleBackColor = true;
			this->b3_RotYdown->Click += gcnew System::EventHandler(this, &MyForm::b3_RotYdown_Click);
			// 
			// b3_RotXup
			// 
			this->b3_RotXup->Location = System::Drawing::Point(512, 43);
			this->b3_RotXup->Name = L"b3_RotXup";
			this->b3_RotXup->Size = System::Drawing::Size(30, 30);
			this->b3_RotXup->TabIndex = 23;
			this->b3_RotXup->Text = L"→";
			this->b3_RotXup->UseVisualStyleBackColor = true;
			this->b3_RotXup->Click += gcnew System::EventHandler(this, &MyForm::b3_RotXup_Click);
			// 
			// b3_RotYup
			// 
			this->b3_RotYup->Location = System::Drawing::Point(482, 17);
			this->b3_RotYup->Name = L"b3_RotYup";
			this->b3_RotYup->Size = System::Drawing::Size(30, 30);
			this->b3_RotYup->TabIndex = 22;
			this->b3_RotYup->Text = L"↑";
			this->b3_RotYup->UseVisualStyleBackColor = true;
			this->b3_RotYup->Click += gcnew System::EventHandler(this, &MyForm::b3_RotYup_Click);
			// 
			// label3ZRot
			// 
			this->label3ZRot->AutoSize = true;
			this->label3ZRot->Location = System::Drawing::Point(362, 84);
			this->label3ZRot->Name = L"label3ZRot";
			this->label3ZRot->Size = System::Drawing::Size(25, 17);
			this->label3ZRot->TabIndex = 21;
			this->label3ZRot->Text = L"Z :";
			// 
			// tb3_ZRot
			// 
			this->tb3_ZRot->Location = System::Drawing::Point(393, 81);
			this->tb3_ZRot->Name = L"tb3_ZRot";
			this->tb3_ZRot->Size = System::Drawing::Size(53, 22);
			this->tb3_ZRot->TabIndex = 20;
			this->tb3_ZRot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_ZRot_KeyPress);
			this->tb3_ZRot->Leave += gcnew System::EventHandler(this, &MyForm::tb3_ZRot_Leave);
			// 
			// tb3_YRot
			// 
			this->tb3_YRot->Location = System::Drawing::Point(393, 53);
			this->tb3_YRot->Name = L"tb3_YRot";
			this->tb3_YRot->Size = System::Drawing::Size(53, 22);
			this->tb3_YRot->TabIndex = 19;
			this->tb3_YRot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_YRot_KeyPress);
			this->tb3_YRot->Leave += gcnew System::EventHandler(this, &MyForm::tb3_YRot_Leave);
			// 
			// label3YRot
			// 
			this->label3YRot->AutoSize = true;
			this->label3YRot->Location = System::Drawing::Point(362, 56);
			this->label3YRot->Name = L"label3YRot";
			this->label3YRot->Size = System::Drawing::Size(25, 17);
			this->label3YRot->TabIndex = 18;
			this->label3YRot->Text = L"Y :";
			// 
			// label3XRot
			// 
			this->label3XRot->AutoSize = true;
			this->label3XRot->Location = System::Drawing::Point(362, 30);
			this->label3XRot->Name = L"label3XRot";
			this->label3XRot->Size = System::Drawing::Size(25, 17);
			this->label3XRot->TabIndex = 17;
			this->label3XRot->Text = L"X :";
			// 
			// tb3_XRot
			// 
			this->tb3_XRot->Location = System::Drawing::Point(393, 25);
			this->tb3_XRot->Name = L"tb3_XRot";
			this->tb3_XRot->Size = System::Drawing::Size(53, 22);
			this->tb3_XRot->TabIndex = 16;
			this->tb3_XRot->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_XRot_KeyPress);
			this->tb3_XRot->Leave += gcnew System::EventHandler(this, &MyForm::tb3_XRot_Leave);
			// 
			// label3Rotare
			// 
			this->label3Rotare->AutoSize = true;
			this->label3Rotare->Location = System::Drawing::Point(340, 7);
			this->label3Rotare->Name = L"label3Rotare";
			this->label3Rotare->Size = System::Drawing::Size(55, 17);
			this->label3Rotare->TabIndex = 15;
			this->label3Rotare->Text = L"Rotare:";
			// 
			// b3_PosXdown
			// 
			this->b3_PosXdown->Location = System::Drawing::Point(240, 44);
			this->b3_PosXdown->Name = L"b3_PosXdown";
			this->b3_PosXdown->Size = System::Drawing::Size(30, 30);
			this->b3_PosXdown->TabIndex = 14;
			this->b3_PosXdown->Text = L"←";
			this->b3_PosXdown->UseVisualStyleBackColor = true;
			this->b3_PosXdown->Click += gcnew System::EventHandler(this, &MyForm::b3_PosXdown_Click);
			// 
			// b3_PosZdown
			// 
			this->b3_PosZdown->Location = System::Drawing::Point(240, 74);
			this->b3_PosZdown->Name = L"b3_PosZdown";
			this->b3_PosZdown->Size = System::Drawing::Size(30, 30);
			this->b3_PosZdown->TabIndex = 13;
			this->b3_PosZdown->Text = L"↙";
			this->b3_PosZdown->UseVisualStyleBackColor = true;
			this->b3_PosZdown->Click += gcnew System::EventHandler(this, &MyForm::b3_PosZdown_Click);
			// 
			// b3_PosZup
			// 
			this->b3_PosZup->Location = System::Drawing::Point(300, 14);
			this->b3_PosZup->Name = L"b3_PosZup";
			this->b3_PosZup->Size = System::Drawing::Size(30, 30);
			this->b3_PosZup->TabIndex = 12;
			this->b3_PosZup->Text = L"↗";
			this->b3_PosZup->UseVisualStyleBackColor = true;
			this->b3_PosZup->Click += gcnew System::EventHandler(this, &MyForm::b3_PosZup_Click);
			// 
			// b3_PosYdown
			// 
			this->b3_PosYdown->Location = System::Drawing::Point(270, 74);
			this->b3_PosYdown->Name = L"b3_PosYdown";
			this->b3_PosYdown->Size = System::Drawing::Size(30, 30);
			this->b3_PosYdown->TabIndex = 11;
			this->b3_PosYdown->Text = L"↓";
			this->b3_PosYdown->UseVisualStyleBackColor = true;
			this->b3_PosYdown->Click += gcnew System::EventHandler(this, &MyForm::b3_PosYdown_Click);
			// 
			// b3_PosXup
			// 
			this->b3_PosXup->Location = System::Drawing::Point(300, 40);
			this->b3_PosXup->Name = L"b3_PosXup";
			this->b3_PosXup->Size = System::Drawing::Size(30, 30);
			this->b3_PosXup->TabIndex = 10;
			this->b3_PosXup->Text = L"→";
			this->b3_PosXup->UseVisualStyleBackColor = true;
			this->b3_PosXup->Click += gcnew System::EventHandler(this, &MyForm::b3_PosXup_Click);
			// 
			// b3_PosYup
			// 
			this->b3_PosYup->Location = System::Drawing::Point(270, 14);
			this->b3_PosYup->Name = L"b3_PosYup";
			this->b3_PosYup->Size = System::Drawing::Size(30, 30);
			this->b3_PosYup->TabIndex = 9;
			this->b3_PosYup->Text = L"↑";
			this->b3_PosYup->UseVisualStyleBackColor = true;
			this->b3_PosYup->Click += gcnew System::EventHandler(this, &MyForm::b3_PosYup_Click);
			// 
			// label3ZPos
			// 
			this->label3ZPos->AutoSize = true;
			this->label3ZPos->Location = System::Drawing::Point(150, 81);
			this->label3ZPos->Name = L"label3ZPos";
			this->label3ZPos->Size = System::Drawing::Size(25, 17);
			this->label3ZPos->TabIndex = 8;
			this->label3ZPos->Text = L"Z :";
			// 
			// tb3_ZPos
			// 
			this->tb3_ZPos->Location = System::Drawing::Point(181, 78);
			this->tb3_ZPos->Name = L"tb3_ZPos";
			this->tb3_ZPos->Size = System::Drawing::Size(53, 22);
			this->tb3_ZPos->TabIndex = 7;
			this->tb3_ZPos->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_ZPos_KeyPress);
			this->tb3_ZPos->Leave += gcnew System::EventHandler(this, &MyForm::tb3_ZPos_Leave);
			// 
			// tb3_YPos
			// 
			this->tb3_YPos->Location = System::Drawing::Point(181, 50);
			this->tb3_YPos->Name = L"tb3_YPos";
			this->tb3_YPos->Size = System::Drawing::Size(53, 22);
			this->tb3_YPos->TabIndex = 6;
			this->tb3_YPos->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_YPos_KeyPress);
			this->tb3_YPos->Leave += gcnew System::EventHandler(this, &MyForm::tb3_YPos_Leave);
			// 
			// label3YPos
			// 
			this->label3YPos->AutoSize = true;
			this->label3YPos->Location = System::Drawing::Point(150, 53);
			this->label3YPos->Name = L"label3YPos";
			this->label3YPos->Size = System::Drawing::Size(25, 17);
			this->label3YPos->TabIndex = 5;
			this->label3YPos->Text = L"Y :";
			// 
			// label3XPos
			// 
			this->label3XPos->AutoSize = true;
			this->label3XPos->Location = System::Drawing::Point(150, 27);
			this->label3XPos->Name = L"label3XPos";
			this->label3XPos->Size = System::Drawing::Size(25, 17);
			this->label3XPos->TabIndex = 4;
			this->label3XPos->Text = L"X :";
			// 
			// tb3_XPos
			// 
			this->tb3_XPos->Location = System::Drawing::Point(181, 22);
			this->tb3_XPos->Name = L"tb3_XPos";
			this->tb3_XPos->Size = System::Drawing::Size(53, 22);
			this->tb3_XPos->TabIndex = 3;
			this->tb3_XPos->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb3_XPos_KeyPress);
			this->tb3_XPos->Leave += gcnew System::EventHandler(this, &MyForm::tb3_XPos_Leave);
			// 
			// label3Position
			// 
			this->label3Position->AutoSize = true;
			this->label3Position->Location = System::Drawing::Point(128, 4);
			this->label3Position->Name = L"label3Position";
			this->label3Position->Size = System::Drawing::Size(62, 17);
			this->label3Position->TabIndex = 2;
			this->label3Position->Text = L"Position:";
			// 
			// ShapeTypeBox3
			// 
			this->ShapeTypeBox3->Enabled = false;
			this->ShapeTypeBox3->FormattingEnabled = true;
			this->ShapeTypeBox3->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Cube", L"Pyramid" });
			this->ShapeTypeBox3->Location = System::Drawing::Point(7, 24);
			this->ShapeTypeBox3->Name = L"ShapeTypeBox3";
			this->ShapeTypeBox3->Size = System::Drawing::Size(113, 24);
			this->ShapeTypeBox3->TabIndex = 1;
			// 
			// label3ShapeType
			// 
			this->label3ShapeType->AutoSize = true;
			this->label3ShapeType->Location = System::Drawing::Point(4, 4);
			this->label3ShapeType->Name = L"label3ShapeType";
			this->label3ShapeType->Size = System::Drawing::Size(84, 17);
			this->label3ShapeType->TabIndex = 0;
			this->label3ShapeType->Text = L"Shape type:";
			// 
			// label3None
			// 
			this->label3None->AutoSize = true;
			this->label3None->Location = System::Drawing::Point(120, 4);
			this->label3None->Name = L"label3None";
			this->label3None->Size = System::Drawing::Size(42, 17);
			this->label3None->TabIndex = 4;
			this->label3None->Text = L"None";
			// 
			// label3Selected
			// 
			this->label3Selected->AutoSize = true;
			this->label3Selected->Location = System::Drawing::Point(4, 4);
			this->label3Selected->Name = L"label3Selected";
			this->label3Selected->Size = System::Drawing::Size(110, 17);
			this->label3Selected->TabIndex = 3;
			this->label3Selected->Text = L"Selected shape:";
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
			// oleDbInsertCommand1
			// 
			this->oleDbInsertCommand1->CommandText = resources->GetString(L"oleDbInsertCommand1.CommandText");
			this->oleDbInsertCommand1->Connection = this->oleDbConnection1;
			this->oleDbInsertCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(13) {
				(gcnew System::Data::OleDb::OleDbParameter(L"x_coor",
					System::Data::OleDb::OleDbType::Integer, 0, L"x_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"y_coor", System::Data::OleDb::OleDbType::Integer,
						0, L"y_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"z_coor", System::Data::OleDb::OleDbType::Integer, 0, L"z_coor")),
						(gcnew System::Data::OleDb::OleDbParameter(L"lenght", System::Data::OleDb::OleDbType::Integer, 0, L"lenght")), (gcnew System::Data::OleDb::OleDbParameter(L"x_rot",
							System::Data::OleDb::OleDbType::Integer, 0, L"x_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"y_rot", System::Data::OleDb::OleDbType::Integer,
								0, L"y_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"z_rot", System::Data::OleDb::OleDbType::Integer, 0, L"z_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"ShapeType",
									System::Data::OleDb::OleDbType::VarWChar, 0, L"ShapeType")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_R", System::Data::OleDb::OleDbType::UnsignedTinyInt,
										0, L"Color_R")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_G", System::Data::OleDb::OleDbType::UnsignedTinyInt,
											0, L"Color_G")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_B", System::Data::OleDb::OleDbType::UnsignedTinyInt,
												0, L"Color_B")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_A", System::Data::OleDb::OleDbType::UnsignedTinyInt,
													0, L"Color_A")), (gcnew System::Data::OleDb::OleDbParameter(L"Scene", System::Data::OleDb::OleDbType::Integer, 0, L"Scene"))
			});
			// 
			// oleDbUpdateCommand1
			// 
			this->oleDbUpdateCommand1->CommandText = resources->GetString(L"oleDbUpdateCommand1.CommandText");
			this->oleDbUpdateCommand1->Connection = this->oleDbConnection1;
			this->oleDbUpdateCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(40) {
				(gcnew System::Data::OleDb::OleDbParameter(L"x_coor",
					System::Data::OleDb::OleDbType::Integer, 0, L"x_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"y_coor", System::Data::OleDb::OleDbType::Integer,
						0, L"y_coor")), (gcnew System::Data::OleDb::OleDbParameter(L"z_coor", System::Data::OleDb::OleDbType::Integer, 0, L"z_coor")),
						(gcnew System::Data::OleDb::OleDbParameter(L"lenght", System::Data::OleDb::OleDbType::Integer, 0, L"lenght")), (gcnew System::Data::OleDb::OleDbParameter(L"x_rot",
							System::Data::OleDb::OleDbType::Integer, 0, L"x_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"y_rot", System::Data::OleDb::OleDbType::Integer,
								0, L"y_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"z_rot", System::Data::OleDb::OleDbType::Integer, 0, L"z_rot")), (gcnew System::Data::OleDb::OleDbParameter(L"ShapeType",
									System::Data::OleDb::OleDbType::VarWChar, 0, L"ShapeType")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_R", System::Data::OleDb::OleDbType::UnsignedTinyInt,
										0, L"Color_R")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_G", System::Data::OleDb::OleDbType::UnsignedTinyInt,
											0, L"Color_G")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_B", System::Data::OleDb::OleDbType::UnsignedTinyInt,
												0, L"Color_B")), (gcnew System::Data::OleDb::OleDbParameter(L"Color_A", System::Data::OleDb::OleDbType::UnsignedTinyInt,
													0, L"Color_A")), (gcnew System::Data::OleDb::OleDbParameter(L"Scene", System::Data::OleDb::OleDbType::Integer, 0, L"Scene")),
													(gcnew System::Data::OleDb::OleDbParameter(L"Original_ID", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
														false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"ID", System::Data::DataRowVersion::Original, nullptr)),
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
																												(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_ShapeType", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																													static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"ShapeType", System::Data::DataRowVersion::Original, true, nullptr)),
																													(gcnew System::Data::OleDb::OleDbParameter(L"Original_ShapeType", System::Data::OleDb::OleDbType::VarWChar, 0, System::Data::ParameterDirection::Input,
																														false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"ShapeType", System::Data::DataRowVersion::Original,
																														nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_R", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																															static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_R", System::Data::DataRowVersion::Original, true, nullptr)),
																															(gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_R", System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input,
																																false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_R", System::Data::DataRowVersion::Original, nullptr)),
																																(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_G", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																																	static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_G", System::Data::DataRowVersion::Original, true, nullptr)),
																																	(gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_G", System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input,
																																		false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_G", System::Data::DataRowVersion::Original, nullptr)),
																																		(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_B", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																																			static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_B", System::Data::DataRowVersion::Original, true, nullptr)),
																																			(gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_B", System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input,
																																				false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_B", System::Data::DataRowVersion::Original, nullptr)),
																																				(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_A", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																																					static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_A", System::Data::DataRowVersion::Original, true, nullptr)),
																																					(gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_A", System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input,
																																						false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Color_A", System::Data::DataRowVersion::Original, nullptr)),
																																						(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Scene", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																																							static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Scene", System::Data::DataRowVersion::Original, true, nullptr)),
																																							(gcnew System::Data::OleDb::OleDbParameter(L"Original_Scene", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
																																								false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Scene", System::Data::DataRowVersion::Original, nullptr))
			});
			// 
			// oleDbDeleteCommand1
			// 
			this->oleDbDeleteCommand1->CommandText = resources->GetString(L"oleDbDeleteCommand1.CommandText");
			this->oleDbDeleteCommand1->Connection = this->oleDbConnection1;
			this->oleDbDeleteCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(27) {
				(gcnew System::Data::OleDb::OleDbParameter(L"Original_ID",
					System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
					static_cast<System::Byte>(0), L"ID", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_x_coor",
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
																			static_cast<System::Byte>(0), L"z_rot", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_ShapeType",
																				System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																				L"ShapeType", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_ShapeType",
																					System::Data::OleDb::OleDbType::VarWChar, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																					static_cast<System::Byte>(0), L"ShapeType", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_R",
																						System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																						L"Color_R", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_R",
																							System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																							static_cast<System::Byte>(0), L"Color_R", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_G",
																								System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																								L"Color_G", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_G",
																									System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																									static_cast<System::Byte>(0), L"Color_G", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_B",
																										System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																										L"Color_B", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_B",
																											System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																											static_cast<System::Byte>(0), L"Color_B", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Color_A",
																												System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																												L"Color_A", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Color_A",
																													System::Data::OleDb::OleDbType::UnsignedTinyInt, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																													static_cast<System::Byte>(0), L"Color_A", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Scene",
																														System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
																														L"Scene", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Scene",
																															System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
																															static_cast<System::Byte>(0), L"Scene", System::Data::DataRowVersion::Original, nullptr))
			});
			// 
			// oleDbDataAdapter1
			// 
			this->oleDbDataAdapter1->DeleteCommand = this->oleDbDeleteCommand1;
			this->oleDbDataAdapter1->InsertCommand = this->oleDbInsertCommand1;
			this->oleDbDataAdapter1->SelectCommand = this->oleDbSelectCommand1;
			cli::array< System::Data::Common::DataColumnMapping^ >^ __mcTemp__1 = gcnew cli::array< System::Data::Common::DataColumnMapping^  >(14) {
				(gcnew System::Data::Common::DataColumnMapping(L"ID",
					L"ID")), (gcnew System::Data::Common::DataColumnMapping(L"x_coor", L"x_coor")), (gcnew System::Data::Common::DataColumnMapping(L"y_coor",
						L"y_coor")), (gcnew System::Data::Common::DataColumnMapping(L"z_coor", L"z_coor")), (gcnew System::Data::Common::DataColumnMapping(L"lenght",
							L"lenght")), (gcnew System::Data::Common::DataColumnMapping(L"x_rot", L"x_rot")), (gcnew System::Data::Common::DataColumnMapping(L"y_rot",
								L"y_rot")), (gcnew System::Data::Common::DataColumnMapping(L"z_rot", L"z_rot")), (gcnew System::Data::Common::DataColumnMapping(L"ShapeType",
									L"ShapeType")), (gcnew System::Data::Common::DataColumnMapping(L"Color_R", L"Color_R")), (gcnew System::Data::Common::DataColumnMapping(L"Color_G",
										L"Color_G")), (gcnew System::Data::Common::DataColumnMapping(L"Color_B", L"Color_B")), (gcnew System::Data::Common::DataColumnMapping(L"Color_A",
											L"Color_A")), (gcnew System::Data::Common::DataColumnMapping(L"Scene", L"Scene"))
			};
			this->oleDbDataAdapter1->TableMappings->AddRange(gcnew cli::array< System::Data::Common::DataTableMapping^  >(1) {
				(gcnew System::Data::Common::DataTableMapping(L"Table",
					L"Shapes", __mcTemp__1))
			});
			this->oleDbDataAdapter1->UpdateCommand = this->oleDbUpdateCommand1;
			// 
			// oleDbConnection1
			// 
			this->oleDbConnection1->ConnectionString = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=D:\\2.Programming\\MyLabWorks\\Lab1\\dat"
				L"abase.mdb";
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
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &MyForm::MyForm_Shown);
			this->MainTabControl->ResumeLayout(false);
			this->CameraTab->ResumeLayout(false);
			this->CameraTab->PerformLayout();
			this->AddShapeTab->ResumeLayout(false);
			this->AddShapeTab->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picture2))->EndInit();
			this->ControlTab->ResumeLayout(false);
			this->ControlTab->PerformLayout();
			this->panel3ControlUI->ResumeLayout(false);
			this->panel3ControlUI->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picture3_Color))->EndInit();
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
		direction.Normalize();

		direction.Normalize();
		target = eyes + direction;
		OpenTK::Matrix4 projection = OpenTK::Matrix4::CreatePerspectiveFieldOfView(float(OpenTK::MathHelper::DegreesToRadians(45.0)), (742.0f / 536.0f), 0.1f, 1000.0f);
		OpenTK::Matrix4 view = OpenTK::Matrix4::LookAt(eyes.X, eyes.Y, eyes.Z, target.X, target.Y, target.Z, up.X, up.Y, up.Z);
		OpenTK::Matrix4 model = OpenTK::Matrix4::Identity;
		OpenTK::Matrix4 MV = view * model;
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);

		GL::MatrixMode(MatrixMode::Projection);
		GL::LoadIdentity();
		GL::LoadMatrix(projection);
		GL::MatrixMode(MatrixMode::Modelview);
		GL::LoadIdentity();
		GL::LoadMatrix(MV);
		GL::Viewport(0, 0, FrameWidth, FrameHeight);

		GL::Enable(EnableCap::DepthTest);
		GL::DepthFunc(DepthFunction::Less);

		float specular[] = { 1.0, 1.0, 1.0, 1.0 };
		float shininess = 50.0;
		GL::ClearColor(float(80 / 255.0), float(80 / 255.0), float(80 / 255.0), 0.1f);
		GL::ShadeModel(ShadingModel::Smooth);
		GL::Material(MaterialFace::Front, MaterialParameter::Specular, specular);
		GL::Material(MaterialFace::Front, MaterialParameter::Shininess, shininess);
		GL::Enable(EnableCap::ColorMaterial);
		GL::Enable(EnableCap::Lighting);
		GL::Enable(EnableCap::Light0);
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);
	}
	private: void LoadData()
	{
		engine.RemoveShapes();
		dataSet1->Reset();
		oleDbSelectCommand1->CommandText = (L"SELECT * FROM Shapes WHERE Scene = " + Convert::ToString(current_scene));
		oleDbDataAdapter1->Fill(dataSet1);

		try { next_id = Convert::ToUInt32(dataSet1->Tables[0]->Rows[dataSet1->Tables[0]->Rows->Count - 1]->ItemArray[0]) + 1; }
		catch (Exception^ ex) { next_id = 1; }

		if (dataSet1->Tables->Count)
		for (int i = 0; i < dataSet1->Tables[0]->Rows->Count; i++)
		{
			ShapeType type = ShapeType::Pyramid;
			String^ t = dataSet1->Tables[0]->Rows[i]->ItemArray[8]->ToString();
			if (t == "Cube") type = ShapeType::Cube;
			engine.CreateShape(type, Color::FromArgb(	Convert::ToInt32(dataSet1->Tables[0]->Rows[i]->ItemArray[12]),
														Convert::ToInt32(dataSet1->Tables[0]->Rows[i]->ItemArray[9]),
														Convert::ToInt32(dataSet1->Tables[0]->Rows[i]->ItemArray[10]),
														Convert::ToInt32(dataSet1->Tables[0]->Rows[i]->ItemArray[11])),
											Convert::ToUInt32(dataSet1->Tables[0]->Rows[i]->ItemArray[4]),
											Convert::ToDouble(dataSet1->Tables[0]->Rows[i]->ItemArray[1]),
											Convert::ToDouble(dataSet1->Tables[0]->Rows[i]->ItemArray[2]),
											Convert::ToDouble(dataSet1->Tables[0]->Rows[i]->ItemArray[3]),
											Convert::ToDouble(dataSet1->Tables[0]->Rows[i]->ItemArray[5]),
											Convert::ToDouble(dataSet1->Tables[0]->Rows[i]->ItemArray[6]),
											Convert::ToDouble(dataSet1->Tables[0]->Rows[i]->ItemArray[7]),
											Convert::ToUInt32(dataSet1->Tables[0]->Rows[i]->ItemArray[0]));
		}
	}
	private: void ControlPanel()
	{
		if (selected)
		{
			panel3ControlUI->Enabled = true;
			label3None->Text = "ID " + Convert::ToString(selected_shape_id);
			current_shape = engine.GetShape(selected_shape_id);

			if (dynamic_cast<Cube^>(current_shape))
			{
				ShapeTypeBox3->SelectedIndex = 0;
				ShapeTypeBox3->Text = "Cube";
			}
			else
			{
				ShapeTypeBox3->SelectedIndex = 1;
				ShapeTypeBox3->Text = "Pyramid";
			}

			colorDialog3->Color = current_shape->GetColor();
			Bitmap^ bmp = gcnew Bitmap(picture3_Color->Width, picture3_Color->Height);
			Graphics^ grf = Graphics::FromImage(bmp);
			grf->Clear(colorDialog3->Color);
			picture3_Color->Image = bmp;
			picture3_Color->Refresh();

			OpenTK::Vector3 pos = current_shape->GetPos();
			tb3_XPos->Text = Convert::ToString(pos.X);
			tb3_YPos->Text = Convert::ToString(pos.Y);
			tb3_ZPos->Text = Convert::ToString(pos.Z);

			OpenTK::Vector3 rot = current_shape->GetDirection();
			tb3_XRot->Text = Convert::ToString(OpenTK::MathHelper::RadiansToDegrees(rot.X));
			tb3_YRot->Text = Convert::ToString(OpenTK::MathHelper::RadiansToDegrees(rot.Y));
			tb3_ZRot->Text = Convert::ToString(OpenTK::MathHelper::RadiansToDegrees(rot.Z));

			tb3_Length->Text = Convert::ToString(current_shape->GetLength());
		}
		else
		{
			panel3ControlUI->Enabled = false;
			label3None->Text = "None";
			ShapeTypeBox3->Text = "";
			tb3_XPos->Text = "";
			tb3_YPos->Text = "";
			tb3_ZPos->Text = "";
			tb3_XRot->Text = "";
			tb3_YRot->Text = "";
			tb3_ZRot->Text = "";
			tb3_Length->Text = "";

			colorDialog3->Color = Color::White;
			Bitmap^ bmp = gcnew Bitmap(picture3_Color->Width, picture3_Color->Height);
			Graphics^ grf = Graphics::FromImage(bmp);
			grf->Clear(Color::White);
			picture3_Color->Image = bmp;
			picture3_Color->Refresh();
		}
	}
	private: void UpdateShape()
	{
		ShapeType type = ShapeType::Cube;
		if (ShapeTypeBox3->SelectedIndex == 1) type = ShapeType::Pyramid;
		engine.ResetShape(type, colorDialog3->Color, Convert::ToUInt32(tb3_Length->Text), Convert::ToDouble(tb3_XPos->Text), Convert::ToDouble(tb3_YPos->Text),
			Convert::ToDouble(tb3_ZPos->Text), Convert::ToDouble(tb3_XRot->Text), Convert::ToDouble(tb3_YRot->Text),Convert::ToDouble(tb3_ZRot->Text), current_shape->GetID());
		current_shape = engine.GetShape(current_shape->GetID());
		DrawAll();
		if (dataSet1->Tables->Count)
		{
			int ind = dataSet1->Tables[0]->Rows->IndexOf(dataSet1->Tables[0]->Select("ID = " + Convert::ToString(current_shape->GetID()))[0]);
			dataSet1->Tables[0]->Rows[ind]->BeginEdit();
			dataSet1->Tables[0]->Rows[ind]->ItemArray[1] = Convert::ToInt32(tb3_XPos->Text);
			dataSet1->Tables[0]->Rows[ind]->ItemArray[2] = Convert::ToInt32(tb3_YPos->Text);
			dataSet1->Tables[0]->Rows[ind]->ItemArray[3] = Convert::ToInt32(tb3_ZPos->Text);
			dataSet1->Tables[0]->Rows[ind]->EndEdit();
			dataSet1->AcceptChanges();
			oleDbDataAdapter1->Update(dataSet1);
		}
	}

	private: void DrawAll()
	{
		target = eyes + direction;
		OpenTK::Matrix4 projection = OpenTK::Matrix4::CreatePerspectiveFieldOfView(float(OpenTK::MathHelper::DegreesToRadians(45.0)), (742.0f / 536.0f), 0.1f, 10000.0f);
		OpenTK::Matrix4 view = OpenTK::Matrix4::LookAt(eyes.X, eyes.Y, eyes.Z, target.X, target.Y, target.Z, up.X, up.Y, up.Z);
		OpenTK::Matrix4 model = OpenTK::Matrix4::Identity;
		OpenTK::Matrix4 MV = view * model;
		GL::Clear(ClearBufferMask::ColorBufferBit | ClearBufferMask::DepthBufferBit);

		GL::MatrixMode(MatrixMode::Projection);
		GL::LoadIdentity();
		GL::LoadMatrix(projection);
		GL::MatrixMode(MatrixMode::Modelview);
		GL::LoadIdentity();
		GL::LoadMatrix(MV);

		DrawOrthPlanes();
		DrawOrthLines();
		engine.DrawProjections(cb1_planeXY->Checked, cb1_planeXZ->Checked, cb1_planeYZ->Checked);
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
			for (double i = -(FrameWidth >> 1); i <= (FrameWidth >> 1); i += (FrameWidth >> 5))
			{
				GL::Vertex3(i, (double)(-FrameHeight >> 1), 0.0);
				GL::Vertex3(i, (double)(FrameHeight >> 1), 0.0);
			}
			for (double i = -(FrameHeight >> 1); i <= (FrameHeight >> 1); i += (FrameHeight >> 5))
			{
				GL::Vertex3((double)(-FrameWidth >> 1), i, 0.0);
				GL::Vertex3((double)(FrameWidth >> 1), i, 0.0);
			}
		}
		if (cb1_planeXZ->Checked)
		{
			GL::Color3(Color::Green);
			for (double i = -(FrameWidth >> 1); i <= (FrameWidth >> 1); i += FrameWidth >> 5)
			{
				GL::Vertex3(i, 0.0, (double)(-FrameDepth >> 1));
				GL::Vertex3(i, 0.0, (double)(FrameDepth >> 1));
			}
			for (double i = -(FrameDepth >> 1); i <= (FrameDepth >> 1); i += FrameDepth >> 5)
			{
				GL::Vertex3((double)(-FrameWidth >> 1), 0.0, i);
				GL::Vertex3((double)(FrameWidth >> 1), 0.0, i);
			}
		}
		if (cb1_planeYZ->Checked)
		{
			GL::Color3(Color::Blue);
			for (double i = -(FrameHeight >> 1); i <= (FrameHeight >> 1); i += FrameHeight >> 5)
			{
				GL::Vertex3(0.0, i, (double)(-FrameDepth >> 1));
				GL::Vertex3(0.0, i, (double)(FrameDepth >> 1));
			}
			for (double i = -(FrameDepth >> 1); i <= (FrameDepth >> 1); i += FrameDepth >> 5)
			{
				GL::Vertex3(0.0, (double)(-FrameHeight >> 1), i);
				GL::Vertex3(0.0, (double)(FrameHeight >> 1), i);
			}
		}
		GL::End();
	}
	private: OpenTK::Vector3 GetRay(double x, double y)
	{
		int viewport[4];
		GL::GetInteger(GetPName::Viewport, viewport);
		double norm_x = double(( x - viewport[2] / 2.0) / (viewport[2] / 2.0));
		double norm_y = double((-y + viewport[3] / 2.0) / (viewport[3] / 2.0));

		OpenTK::Vector3 left_dir(up.Y*direction.Z - up.Z*direction.Y, -(up.Z*direction.Y - up.X*direction.Z), up.X*direction.Y - up.Y*direction.X);
		OpenTK::Vector3 up_dir(left_dir.Y*direction.Z - left_dir.Z*direction.Y, -(left_dir.Z*direction.Y - left_dir.X*direction.Z), left_dir.X*direction.Y - left_dir.Y*direction.X);
		left_dir.Normalize();
		up_dir.Normalize();
		
		left_dir *= float((371.0 / 650.0)*norm_x);
		up_dir   *= float((268.0 / 650.0)*norm_y);

		OpenTK::Vector3 res = direction;
		res += -left_dir;
		res += up_dir;
		return res;
	}

	private: System::Void MyForm_Shown(System::Object^  sender, System::EventArgs^  e)
	{
		InitializeGL();
		LoadData();
		DrawAll();
		DrawAll();

		colorDialog->Color = Color::Red;
		Bitmap^ bmp = gcnew Bitmap(picture2->Width, picture2->Height);
		Graphics^ grf = Graphics::FromImage(bmp);
		grf->Clear(Color::Red);
		picture2->Image = bmp;
		picture2->Refresh();

		colorDialog3->Color = Color::White;
		Bitmap^ bmp1 = gcnew Bitmap(picture3_Color->Width, picture3_Color->Height);
		Graphics^ grf1 = Graphics::FromImage(bmp1);
		grf->Clear(Color::White);
		picture3_Color->Image = bmp1;
		picture3_Color->Refresh();
	}
	private: System::Void GLFrame_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if (e->KeyChar == 27)
		{
			engine.DeselectAll();
			selected = false;
			ControlPanel();
		}
		engine.CameraControl(e);
		OpenTK::Vector3 light = engine.GetLight();
		tb1_LightX->Text = Convert::ToString(light.X);
		tb1_LightY->Text = Convert::ToString(light.Y);
		tb1_LightZ->Text = Convert::ToString(light.Z);
		DrawAll();
	}
	private: System::Void GLFrame_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		engine.DeselectAll();
		if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			float x = float(MousePosition.X - this->Left - GLFrame->Left -  8);
			float y = float(MousePosition.Y - this->Top  - GLFrame->Top  - 31);
			OpenTK::Vector3 ray = GetRay(x, y);
			int selected_id = engine.Intersection(ray, eyes);
			if (selected_id != -1)
			{
				selected_shape_id = selected_id;
				engine.SelectShape(selected_id);
			}
		}
		ControlPanel();
		DrawAll();
	}
	private: System::Void MyForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
	{
		oleDbDataAdapter1->Update(dataSet1);
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
		if (tb1_scal->Text->Length == 0 || tb1_scal->Text == "-") tb1_scal->Text = "5";
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
		else if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
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
	private: System::Void tb1_Scene_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb1_Scene->Text->Length == 0) tb1_Scene->Text = "0";
		current_scene = Convert::ToInt32(tb1_Scene->Text);
		LoadData();
		DrawAll();
	}
	private: System::Void tb1_Scene_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13 && tb1_Scene->Text->Length)
		{
			current_scene = Convert::ToInt32(tb1_Scene->Text);
			LoadData();
			DrawAll();
		}
		else if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb1_rot->Text->Length != 0) e->Handled = true;
		}
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
	private: System::Void b2_setColor_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		colorDialog->ShowDialog();
		Bitmap^ bmp = gcnew Bitmap(picture2->Width, picture2->Height);
		Graphics^ grf = Graphics::FromImage(bmp);
		grf->Clear(colorDialog->Color);
		picture2->Image = bmp;
		picture2->Refresh();
	}
	private: System::Void but_AddShape_Click(System::Object^  sender, System::EventArgs^  e)
	{
		SystemSounds::Beep->Play();
		ShapeType type;
		String^ b = "Cube";
		if (ShapeListBox2->SelectedItem == ShapeListBox2->Items[0]) type = ShapeType::Cube;
		else if (ShapeListBox2->SelectedItem == ShapeListBox2->Items[1]) { type = ShapeType::Pyramid; b = "Pyramid"; }
		engine.CreateShape(type, colorDialog->Color,	Convert::ToUInt32(tb2_leght->Text),
														Convert::ToDouble(tb2_Xcoor->Text),
														Convert::ToDouble(tb2_Ycoor->Text),
														Convert::ToDouble(tb2_Zcoor->Text),
														Convert::ToDouble(tb2_Xrot->Text),
														Convert::ToDouble(tb2_Yrot->Text),
														Convert::ToDouble(tb2_Zrot->Text), 
														next_id);
		if (dataSet1->Tables->Count)
		{
			dataSet1->Tables[0]->Rows->Add(	next_id,
											tb2_Xcoor->Text, 
											tb2_Ycoor->Text, 
											tb2_Zcoor->Text,
											tb2_leght->Text, 
											tb2_Xrot->Text, 
											tb2_Yrot->Text, 
											tb2_Zrot->Text, 
											b, 
											Convert::ToInt32(colorDialog->Color.R), 
											Convert::ToInt32(colorDialog->Color.G),
											Convert::ToInt32(colorDialog->Color.B),
											Convert::ToInt32(colorDialog->Color.A),
											current_scene);
			oleDbDataAdapter1->Update(dataSet1);
		}
		next_id++;
		DrawAll();
	}

	private: System::Void b3_Color_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		colorDialog3->ShowDialog();
		Bitmap^ bmp = gcnew Bitmap(picture3_Color->Width, picture3_Color->Height);
		Graphics^ grf = Graphics::FromImage(bmp);
		grf->Clear(colorDialog3->Color);
		picture3_Color->Image = bmp;
		picture3_Color->Refresh();
		UpdateShape();
	}
	private: System::Void b3_PosXdown_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_XPos->Text = Convert::ToString(Convert::ToDouble(tb3_XPos->Text) - Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_PosYdown_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_YPos->Text = Convert::ToString(Convert::ToDouble(tb3_YPos->Text) - Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_PosZdown_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_ZPos->Text = Convert::ToString(Convert::ToDouble(tb3_ZPos->Text) - Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_PosXup_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_XPos->Text = Convert::ToString(Convert::ToDouble(tb3_XPos->Text) + Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_PosYup_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_YPos->Text = Convert::ToString(Convert::ToDouble(tb3_YPos->Text) + Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_PosZup_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_ZPos->Text = Convert::ToString(Convert::ToDouble(tb3_ZPos->Text) + Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_RotXdown_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_XRot->Text = Convert::ToString(Convert::ToDouble(tb3_XRot->Text) - Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_RotYdown_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_YRot->Text = Convert::ToString(Convert::ToDouble(tb3_YRot->Text) - Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_RotZdown_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_ZRot->Text = Convert::ToString(Convert::ToDouble(tb3_ZRot->Text) - Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_RotXup_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_XRot->Text = Convert::ToString(Convert::ToDouble(tb3_XRot->Text) + Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_RotYup_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_YRot->Text = Convert::ToString(Convert::ToDouble(tb3_YRot->Text) + Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_RotZup_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_ZRot->Text = Convert::ToString(Convert::ToDouble(tb3_ZRot->Text) + Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_LengthDown_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if (Convert::ToUInt32(tb3_Length->Text) < Convert::ToUInt32(tb3_Step->Text)) return;
		tb3_Length->Text = Convert::ToString(Convert::ToUInt32(tb3_Length->Text) - Convert::ToUInt32(tb3_Step->Text));
		UpdateShape();
	}
	private: System::Void b3_LengthUp_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		tb3_Length->Text = Convert::ToString(Convert::ToDouble(tb3_Length->Text) + Convert::ToDouble(tb3_Step->Text));
		UpdateShape();
	}

	private: System::Void tb3_Step_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
			e->Handled = true;
	}
	private: System::Void tb3_XPos_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) UpdateShape();
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb3_XPos->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb3_YPos_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) UpdateShape();
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb3_YPos->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb3_ZPos_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) UpdateShape();
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb3_ZPos->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb3_XRot_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) UpdateShape();
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb3_XRot->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb3_YRot_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) UpdateShape();
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb3_YRot->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb3_ZRot_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (e->KeyChar == 13) UpdateShape();
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
		{
			if (tb3_ZRot->Text->Length != 0 || e->KeyChar != 45)
				e->Handled = true;
		}
	}
	private: System::Void tb3_Length_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8)
			e->Handled = true;
	}
	private: System::Void tb3_Step_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_Step->Text->Length == 0) tb3_Step->Text = "5";
	}
	private: System::Void tb3_XPos_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_XPos->Text->Length == 0) tb3_XPos->Text = "0";
		UpdateShape();
	}
	private: System::Void tb3_YPos_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_YPos->Text->Length == 0) tb3_YPos->Text = "0";
		UpdateShape();
	}
	private: System::Void tb3_ZPos_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_ZPos->Text->Length == 0) tb3_ZPos->Text = "0";
		UpdateShape();
	}
	private: System::Void tb3_XRot_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_XRot->Text->Length == 0) tb3_XRot->Text = "0";
		UpdateShape();
	}
	private: System::Void tb3_YRot_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_YRot->Text->Length == 0) tb3_YRot->Text = "0";
		UpdateShape();
	}
	private: System::Void tb3_ZRot_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_ZRot->Text->Length == 0) tb3_ZRot->Text = "0";
		UpdateShape();
	}
	private: System::Void tb3_Length_Leave(System::Object^  sender, System::EventArgs^  e) 
	{
		if (tb3_Length->Text->Length == 0) tb3_Length->Text = "50";
		UpdateShape();
	}

	private: System::Void b3_deleteShape_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		engine.DeleteShape(selected_shape_id);
		if (dataSet1->Tables->Count)
		{
			int ind = dataSet1->Tables[0]->Rows->IndexOf(dataSet1->Tables[0]->Select("ID = " + Convert::ToString(current_shape->GetID()))[0]);
			dataSet1->Tables[0]->Rows->RemoveAt(ind);
			oleDbDataAdapter1->Update(dataSet1);
		}
		selected = false;
		ControlPanel();
		DrawAll();
	}
};
}