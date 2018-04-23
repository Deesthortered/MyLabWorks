#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

namespace Lab2 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MyForm : public System::Windows::Forms::Form
	{


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
	private: System::ComponentModel::Container ^components;
#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(806, 532);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion


	};
}
