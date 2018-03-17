#include "MyForm.h"
#include <GL\glut.h>

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Lab1::MyForm Form;
	Application::Run(%Form);
}
