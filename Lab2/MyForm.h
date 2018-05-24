#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

namespace Lab2 
{
	using namespace System;
	using namespace System::Data; 
	using namespace System::Data::OleDb;
	using namespace System::Collections::Generic;
	using namespace System::Collections::Concurrent;
	using namespace System::Windows::Forms;
	using namespace System::Threading;
	using namespace System::Runtime::InteropServices;

	public ref class MyForm : public System::Windows::Forms::Form
	{
		ref class WinSocket
		{
			static WSADATA *ws;
			SOCKET main_socket;
			sockaddr_in *adr;
			std::vector<SOCKET> *sockets;

			char* ip;
			u_short port;
			size_t max_client_count;
			size_t client_count;
		public:
			WinSocket()
			{
				this->sockets = new std::vector<SOCKET>();
				this->sockets->reserve(100);
				this->adr = new sockaddr_in;
				ZeroMemory(this->adr, sizeof(this->adr));
				this->ip = nullptr;
				this->max_client_count = SOMAXCONN;
				this->client_count = 0;
			}
			~WinSocket()
			{
				CloseSocket();
				delete this->sockets;
				delete this->adr;
				if (this->ip) delete[] this->ip;
			}

			static bool InitializeLibrary()
			{
				ws = new WSADATA;
				if (FAILED(WSAStartup(MAKEWORD(2, 0), ws))) return false;
				return true;
			}
			static bool CloseLibrary()
			{
				if (FAILED(WSACleanup())) return false;
				delete ws;
				return true;
			}
			bool InitializeSocket()
			{
				if (INVALID_SOCKET == (main_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)))
				{
					closesocket(this->main_socket);
					return false;
				}
				return true;
			}
			void CloseSocket()
			{
				for (size_t i = 0; i < this->sockets->size(); i++)
				{
					shutdown(this->sockets->at(i), 0);
					closesocket(this->sockets->at(i));
				}
				this->sockets->clear();
				shutdown(this->main_socket, 0);
				closesocket(this->main_socket);
			}

			String^ GetIP()
			{
				return gcnew String(this->ip);
			}
			size_t GetPort()
			{
				return this->port;
			}
			void Set_IP(String ^ip)
			{
				if (this->ip) delete[] this->ip;
				this->ip = new char[16];
				char *k = (char*)(void*)Marshal::StringToHGlobalAnsi(ip);
				strcpy(this->ip, k);
				this->ip[15] = '\0';

				ZeroMemory(this->adr, sizeof(this->adr));
				adr->sin_family = AF_INET;
				adr->sin_addr.S_un.S_addr = inet_addr(this->ip);
			}
			void Set_Port(u_short port)
			{
				this->port = port;
				adr->sin_port = htons(port);
			}
			void Set_IPAuto()
			{
				ZeroMemory(this->adr, sizeof(this->adr));
				adr->sin_family = AF_INET;
				hostent* hn;
				hn = gethostbyname(INADDR_ANY);
				this->adr->sin_addr.S_un.S_addr = *(DWORD*)hn->h_addr_list[0];
			}
			void Set_PortAuto()
			{
				this->adr->sin_port = htons(0);
			}
			size_t GetClientCount()
			{
				return this->max_client_count;
			}
			void SetClientCount(size_t c)
			{
				this->max_client_count = c;
				this->sockets->reserve(c);
			}
			bool Bind()
			{
				sockaddr_in a = *this->adr;
				if (bind(this->main_socket, (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) return false;
				this->updateIpPort();
				return true;
			}
			bool Connect()
			{
				sockaddr_in a = *this->adr;
				if (SOCKET_ERROR == (connect(main_socket, (sockaddr *)&a, sizeof(a)))) 
					return false;
				return true;
			}
			bool Disconnect()
			{
				if (SOCKET_ERROR == (shutdown(main_socket, 0))) return false;
				closesocket(this->main_socket);
				return true;
			}
			bool DisconnectClient(SOCKET sock)
			{
				if (SOCKET_ERROR == (shutdown(sock, 0))) return false;
				closesocket(sock);
				for (size_t i = 0; i < this->sockets->size(); i++)
					if (this->sockets->at(i) == sock)
					{
						this->sockets->erase(this->sockets->begin() + i);
						return true;
					}
				return false;
			}
			bool Listen()
			{
				if (FAILED(listen(this->main_socket, this->max_client_count))) 
					return false;
				return true;
			}
			SOCKET Accept()
			{
				this->sockets->push_back(accept(this->main_socket, 0, 0));
				if (FAILED(this->sockets->back()))
				{
					this->sockets->pop_back();
					return SOCKET_ERROR;
				}
				return this->sockets->back();
			}
			bool GetData(char *data, size_t data_size)
			{
				int len = recv(this->main_socket, data, data_size, 0);
				if ((len == SOCKET_ERROR) || (len == 0)) return false;
				return true;
			}
			bool SendData(char *data, size_t data_size)
			{
				if (SOCKET_ERROR == send(this->main_socket, data, data_size, 0)) return false;
				return true;
			}
			bool GetData(char *data, size_t data_size, SOCKET s)
			{
				int len = recv(s, data, data_size, 0);
				if ((len == SOCKET_ERROR) || (len == 0)) return false;
				return true;
			}
			bool SendData(char *data, size_t data_size, SOCKET s)
			{
				if (SOCKET_ERROR == send(s, data, data_size, 0)) return false;
				return true;
			}
		private:
			void updateIpPort()
			{
				sockaddr_in name;
				int n_l = sizeof(name);
				ZeroMemory(&name, sizeof(name));
				getsockname(this->main_socket, (sockaddr*)&name, &n_l);
				if (this->ip) delete[] this->ip;
				this->ip = new char[16];
				ip[15] = '\0';
				strcpy(this->ip, inet_ntoa((in_addr)name.sin_addr));
				this->port = ntohs(name.sin_port);
			}
		};
		ref class Interface
		{
			const bool switch_on = true;
			Label ^infoline;
			Label ^indicator;
			ListBox ^members;
			RichTextBox ^allMessages;
			RichTextBox ^currMessage;
			bool indic_b;
			DataSet ^dataSet;
			OleDbDataAdapter ^dataAdapter;
			size_t last_row_key = 1;
		public:
			Interface(Label ^%infoline, Label ^%indicator, ListBox ^%members, RichTextBox ^%allMessages, RichTextBox ^%currMessage, DataSet ^%dataSet, OleDbDataAdapter ^%dataAdapter)
			{
				this->infoline = infoline;
				this->indicator = indicator;
				this->members = members;
				this->allMessages = allMessages;
				this->currMessage = currMessage;
				this->dataSet = dataSet;
				this->dataAdapter = dataAdapter;
			}
			void Indicate()
			{
				this->indic_b = !this->indic_b;
				if (this->switch_on)
				{
					if (this->indic_b) indicator->Text = "+++";
					else indicator->Text = "---";
				}
			}
			void Deindicate()
			{
				if (this->switch_on) indicator->Text = "Indicator";
			}
			void InfoLine(String ^s)
			{
				if (this->switch_on) infoline->Text = s;
			}
			void AddMemberName(String ^s)
			{
				if (this->switch_on) this->members->Items->Add(s + "\n");
			}
			void DeleteMemberName(String ^name)
			{
				if (this->switch_on) this->members->Items->Remove(name);
			}
			void ClearMemberBox()
			{
				if (this->switch_on) this->members->Items->Clear();
			}

			void Message(String ^s)
			{
				if (this->switch_on) this->allMessages->Text += (s + "\n");
			}
			void MessageError(String ^who, String ^what)
			{
				if (this->switch_on) this->allMessages->Text += ("Ошибка!!! (" + who + "): \t" + what + "\n");
			}
			void MessageNewMember(String ^who)
			{
				if (this->switch_on) this->allMessages->Text += (who + " подключился к чату.\n");
			}
			void MessageUser(String ^s)
			{
				if (this->switch_on) this->allMessages->Text += (s);
			}
			void ClearMessages()
			{
				if (this->switch_on) this->allMessages->Text = "";
			}

			String^ CurrentMessageGet()
			{
				return this->currMessage->Text;
			}
			void CurrentMessageClear()
			{
				if (this->switch_on) this->currMessage->Text = "";
			}

			void AddDataToBase(String ^who, String ^what)
			{
				//if (!this->switch_on) return;
				DataRow ^row = this->dataSet->Tables[0]->NewRow();
				row["Counter"] = this->last_row_key;
				row["Time"] = DateTime::Today;
				row["Sender"] = who;
				row["Message"] = what;
				dataSet->Tables[0]->Rows->Add(row);
				dataAdapter->Update(this->dataSet);
			}
			List<String^>^ LoadAllMessages()
			{
				List<String^>^ res = gcnew List<String^>();
				for (int i = 0; i < this->dataSet->Tables[0]->Rows->Count; i++)
				{
					String ^who, ^what;
					who  = (String^)this->dataSet->Tables[0]->Rows[i]->ItemArray[2];
					what = (String^)this->dataSet->Tables[0]->Rows[i]->ItemArray[3];
					if (who == "System") res->Add(what + "\n");
					else res->Add(who + ": \t" + what + "\n");
				}
				this->last_row_key = this->dataSet->Tables[0]->Rows->Count + 1;
				return res;
			}
		};
		ref class Engine
		{
		protected:
			Interface ^face;
			String ^name;
			String ^ip;
			String ^port;

			WinSocket ^Socket;
			bool is_active;
			bool is_server;

			const size_t buff_len = 5120;
			char *buff = nullptr;
			char* r_error =			"ERROR";
			char* r_hello1 =		"Hey!\0";
			char* r_hello2 =		"Yo!\0";
			char* r_ok =			"OK\0";
			char* r_end_names =		"ENDNAMES\0";
			char* r_update =		"UPDATE\0";
			char* r_end_update =	"ENDUPDATE\0";
			char* r_disconnect =	"DISCONNECT\0";
			char* r_repeat =		"REPEAT\0";
			char* r_sendmessage =	"SENDMESSAGE\0";
			char* r_alreadyexist =	"ALREADYEXIST\0";

		public:
			Engine()
			{
				this->is_active = false;
				this->buff = nullptr;
			}
			Engine(Interface ^face)
			{
				this->is_active = false;
				this->buff = nullptr;
				this->face = face;
			}
			~Engine()
			{
				if (buff) delete[] buff;
			}

			String^ GetIP()
			{
				return this->ip;
			}
			String^ GetPort()
			{
				return this->port;
			}
			bool IsActive()
			{
				return this->is_active;
			}
			bool IsServer()
			{
				return this->is_server;
			}
			static void ErrorMessage(int k)
			{
				switch (k)
				{
				case 0: { MessageBox::Show("Не удалось проинициализировать библиотеку WinSock. Функция WSAStartup выполнена некорректно.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 1: { MessageBox::Show("Не удалось выгрузить библиотеку WinSock. Функция WSACleanup выполнена некорректно.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 2: { MessageBox::Show("Не удалось проинициализировать серверный сокет.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 3: { MessageBox::Show("Не удалось забиндить сокет сервера", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 4: { MessageBox::Show("Не удалось поставить сокет сервера на слушающий режим", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 5: { MessageBox::Show("Не удалось проинициализировать клиентский сокет.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 6: { MessageBox::Show("Не удалось подключится к указаному адресу.", "Ошибка подключения!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 7: { MessageBox::Show("Подключение произошло, но не было подтверждено.", "Ошибка подключения!", MessageBoxButtons::OK, MessageBoxIcon::Warning); } break;
				case 8: { MessageBox::Show("Поле IP и порта не может быть пустым. Введите данные.", "Внимание!", MessageBoxButtons::OK, MessageBoxIcon::Warning); } break;
				case 10: { MessageBox::Show("Такое имя уже занято на сервере.", "Внимание!", MessageBoxButtons::OK, MessageBoxIcon::Warning); } break;
				default: MessageBox::Show("Неизвестная фатально-летальная ошибка №" + Convert::ToString(k), "ERRORЩИНА!!!", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}

			bool virtual Start() { return false; }
			void virtual Terminate() {}
			void virtual Disconnect() {}
			void virtual SendMessage() {}
		protected:
			void ClearBuffer()
			{
				memset(buff, 0, buff_len);
			}
		};
		ref class Server : public Engine
		{
			Thread^ listen_thr;
			ConcurrentBag<SOCKET> client_sockets;
			ConcurrentDictionary<SOCKET, Thread^> client_threads;
			ConcurrentDictionary<SOCKET, String^> names;
			ConcurrentDictionary<SOCKET, ConcurrentQueue<String^>^> client_data;
			bool lockedBag;

		public:
			Server(String ^name, String ^ip, String ^port, Interface ^face)
			{
				this->name = name;
				this->ip = ip;
				this->port = port;
				this->is_server = true;
				this->face = face;
				this->lockedBag = false;
			}

			virtual bool Start() override
			{
				this->is_active = true;
				Socket = gcnew WinSocket();
				buff = new char[buff_len];
				ClearBuffer();
				face->InfoLine("Инициализация серверного сокета...");
				if (!Socket->InitializeSocket())
				{
					ErrorMessage(2);
					Terminate();
					return false;
				}
				Socket->Set_IPAuto();
				Socket->Set_PortAuto();
				face->InfoLine("Биндим сокет на автоматический адресс...");
				if (!Socket->Bind())
				{
					ErrorMessage(3);
					Terminate();
					return false;
				}
				this->ip = Socket->GetIP();
				this->port = Socket->GetPort().ToString();
				face->InfoLine("Ставим серверный сокет в слушающий режим...");
				if (!Socket->Listen())
				{
					ErrorMessage(4);
					Terminate();
					return false;
				}
				face->InfoLine("Запускаем поток обработки клиентских соединений.");
				listen_thr = gcnew Thread(gcnew ThreadStart(this, &Server::Listening));
				listen_thr->Start();
				face->InfoLine("Готово!");
				face->Message("Сервер создан. Имя сервера: " + name);
				face->AddMemberName(name);

				List<String^> ^allmsg = face->LoadAllMessages();
				for (int i = 0; i < allmsg->Count; i++)
					face->MessageUser(allmsg[i]);

				return true;
			}
			virtual void Terminate() override
			{
				is_active = false;
				if (listen_thr != nullptr)
					listen_thr->Abort();
				while (this->lockedBag) {};
				List<SOCKET> ^sock_arr = gcnew List<SOCKET>(this->client_sockets.ToArray());
				for (int i = 0; i < sock_arr->Count; i++)
					if (this->client_threads[sock_arr[i]] != nullptr)
						this->client_threads[sock_arr[i]]->Abort();
				Socket->~WinSocket();
				delete[] buff;

				while (this->lockedBag) {};
				while (!client_sockets.IsEmpty)
				{
					SOCKET s;
					this->client_sockets.TryTake(s);
				}
				this->client_threads.Clear();
				this->names.Clear();
				this->client_data.Clear();
				face->InfoLine("Выберите режим работы программы.");
				face->ClearMemberBox();
			}
			virtual void SendMessage() override
			{
				while (this->lockedBag) {};
				List<SOCKET> ^sock_arr = gcnew List<SOCKET>(this->client_sockets.ToArray());
				String ^k = face->CurrentMessageGet();
				String ^s = (this->name + ": \t" + k + "\n");
				face->CurrentMessageClear();
				for (int i = 0; i < sock_arr->Count; i++)
					this->client_data[sock_arr[i]]->Enqueue(s);
				face->MessageUser(s);
				face->AddDataToBase(this->name, k);
			}

		private:
			void Listening()
			{
				while (this->is_active)
				{
					SOCKET s = Socket->Accept();
					if (s == SOCKET_ERROR) continue;
					while (this->lockedBag) {};
					client_sockets.Add(s);
					this->client_threads[s] = gcnew Thread(gcnew ParameterizedThreadStart(this, &Server::ConnectionFunc));
					this->client_data[s] = gcnew ConcurrentQueue<String^>();
					client_threads[s]->Start(s);
				}
			}
			void ConnectionFunc(Object^ obj)
			{
				SOCKET sock = (SOCKET)obj;
				if (!Socket->SendData(r_hello1, strlen(r_hello1), sock)) { ConnectionError(sock, -5); return; }
				if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, -4); return; }
				if (strcmp(r_hello2, buff)) { ConnectionError(sock, -3); return; }
				if (!Socket->SendData(r_ok, strlen(r_ok), sock)) { ConnectionError(sock, -2); return; }

				// Получаем имя клиента
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, -1); return; }
				String ^client_name = gcnew String(buff);

				if (client_name == this->name)
				{
					if (!Socket->SendData(r_alreadyexist, strlen(r_alreadyexist), sock)) { ConnectionError(sock, 0); return; }
					disconnectclient(sock);
					return;
				}
				List<KeyValuePair<SOCKET, String^>>^ rr = gcnew List<KeyValuePair<SOCKET, String^>>(this->names.ToArray());
				for (int i = 0; i < rr->Count; i++)
					if (rr[i].Value == client_name)
					{
						if (!Socket->SendData(r_alreadyexist, strlen(r_alreadyexist), sock)) { ConnectionError(sock, 0); return; }
						disconnectclient(sock);
						return;
					}

				this->names[sock] = client_name;
				face->AddMemberName(client_name);
				face->MessageNewMember(client_name);

				// Отсылаем ему список всех участников
				while (this->lockedBag) {};
				List<SOCKET> ^sock_arr = gcnew List<SOCKET>(this->client_sockets.ToArray());

				char *k = (char*)(void*)Marshal::StringToHGlobalAnsi(this->name);
				if (!Socket->SendData(k, this->name->Length, sock)) { ConnectionError(sock, 1); return; }
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, 2); return; }
				if (strcmp(buff, r_ok)) { ConnectionError(sock, 2); return; }

				for (int i = 0; i < sock_arr->Count; i++)
				{
					char *k = (char*)(void*)Marshal::StringToHGlobalAnsi(this->names[sock_arr[i]]);
					if (!Socket->SendData(k, this->names[sock_arr[i]]->Length, sock)) { ConnectionError(sock, 1); return; }
					ClearBuffer();
					if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, 2); return; }
					if (strcmp(buff, r_ok)) { ConnectionError(sock, 2); return; }
				}
				if (!Socket->SendData(r_end_names, strlen(r_end_names), sock)) { ConnectionError(sock, 3); return; }
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, 4); return; }
				if (strcmp(buff, r_ok)) { ConnectionError(sock, 4); return; }
				if (!Socket->SendData(r_ok, strlen(r_ok), sock)) { ConnectionError(sock, 5); return; }

				for (int i = 0; i < sock_arr->Count; i++)
					if (this->names[sock_arr[i]] != client_name)
					{
						String ^s = "NEW" + this->names[sock_arr[i]];
						this->client_data[sock_arr[i]]->Enqueue(s);
					}

				List<String^> ^allmsg = face->LoadAllMessages();
				for (int i = 0; i < allmsg->Count; i++)
					this->client_data[sock]->Enqueue(allmsg[i]);

				// Цикл приема и обработки запросов.
				while (this->is_active)
				{
					ClearBuffer();
					if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, 6); return; }

					if (!strcmp(buff, r_update)) RequestUpdate(sock);
					else if (!strcmp(buff, r_disconnect))
					{
						RequestDisconect(sock);
						return;
					}
					else if (!strcmp(buff, r_sendmessage))
					{
						RequestSendMessage(sock, client_name);
					}
					else if (!strlen(buff))
					{
						if (!Socket->SendData(r_repeat, strlen(r_repeat), sock))
							ConnectionError(sock, 7); return;
					}
					else
					{
						if (!Socket->SendData(r_error, strlen(r_error), sock))
							ConnectionError(sock, 8); return;
						face->MessageError(client_name, "UNKNOWN REQUEST");
					}
				}
			}

			void RequestUpdate(SOCKET sock)
			{
				while (!this->client_data[sock]->IsEmpty)
				{
					String ^s; this->client_data[sock]->TryDequeue(s);
					char *k = (char*)(void*)Marshal::StringToHGlobalAnsi(s);
					if (!Socket->SendData(k, s->Length, sock)) { ConnectionError(sock, 9); return; };
					ClearBuffer();
					if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, 10); return; };
					if (strcmp(buff, r_ok)) { ConnectionError(sock, 11); return; };
				}
				if (!Socket->SendData(r_end_update, strlen(r_end_update), sock))
					{ ConnectionError(sock, 12); return; };
			}
			void RequestSendMessage(SOCKET sock, String ^cl_n)
			{
				if (!Socket->SendData(r_ok, strlen(r_ok), sock)) { ConnectionError(sock, 13); return; };
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len, sock)) { ConnectionError(sock, 14); return; };
				String ^s = gcnew String(buff);
				while (this->lockedBag) {};
				List<SOCKET> ^sock_arr = gcnew List<SOCKET>(this->client_sockets.ToArray());
				for (int i = 0; i < sock_arr->Count; i++)
					this->client_data[sock_arr[i]]->Enqueue((cl_n + ": \t" + s + "\n"));
				face->Message(cl_n + ": \t" + s);
				if (!Socket->SendData(r_ok, strlen(r_ok), sock)) { ConnectionError(sock, 15); return; };
				face->AddDataToBase(this->names[sock], s);
			}
			void RequestDisconect(SOCKET sock)
			{
				if (!Socket->SendData(r_ok, strlen(r_ok), sock)) { ConnectionError(sock, 16); return; };
				face->Message((this->names.ContainsKey(sock) ? this->names[sock] : "кто-то") + " покинул чат.");
				disconnectclient(sock);
			}

			void ConnectionError(SOCKET sock, int errnum)
			{
				this->face->Message("Проблемы с подключением с " + (this->names.ContainsKey(sock) ? this->names[sock] : "новым подключением") + ", потому он отключен от чата.");
				String ^reason;
				switch (errnum)
				{
				case -5: { reason = "Не удалось отправить приветствие"; } break;
				case -4: { reason = "Не удалось принять приветствие"; } break;
				case -3: { reason = "Принято некорректное приветствие"; } break;
				case -2: { reason = "Не удалось отправить ОК"; } break;
				case -1: { reason = "Не удалось получить имя клиента"; } break;
				case 0:  { reason = "Не удалось отправить запрос конфликта имен"; } break;
				case 1:  { reason = "Не удалось отправить клиенту имя участника"; } break;
				case 2:  { reason = "Не удалось получить подтверждение получения имени участника клиентом"; } break;
				case 3:  { reason = "Не удалось отправить запрос на завершение приема имен участников"; } break;
				case 4:  { reason = "Не удалось получить подтверждение на завершение приема имен участников"; } break;
				case 5:  { reason = "Не удалось отправить последний серверный запрос перед запуском цикла запросов"; } break;
				case 6:  { reason = "Не удалось получить запрос от клиента (даже обновляющий)"; } break;
				case 7:  { reason = "Был получен пустой запрос и не удалось отправить запрос на повтор отправки данных"; } break;
				case 8:  { reason = "Не удалось отправить запрос об ошибке при получении некорректного запроса от клиента"; } break;
				case 9:  { reason = "Не удалось отправить последний запрос из очереди запросов"; } break;
				case 10: { reason = "Не удалось получить подтверждение приема запроса клиентом"; } break;
				case 11: { reason = "Вместо подтверждения приема запроса клиентом был получен некорректный запрос"; } break;
				case 12: { reason = "Не удалось отправить запрос окончания приема данных с очереди запросов"; } break;
				case 13: { reason = "Не удалось подтвердить начало приема сообщения"; } break;
				case 14: { reason = "Не удалось принять сообщение"; } break;
				case 15: { reason = "Не удалось подтвердить конец приема сообщения"; } break;
				case 16: { reason = "Ошибка при подтверждении отключения соединения"; } break;
				}
				this->face->Message("Причина " + Convert::ToString(errnum) + ": " + reason + ".");
				disconnectclient(sock);
			}
			void disconnectclient(SOCKET sock)
			{
				Socket->DisconnectClient(sock);
				if (this->names.ContainsKey(sock))
				{
					face->DeleteMemberName(this->names[sock]);
					String ^s;  this->names.TryRemove(sock, s);
				}
				ConcurrentQueue<String^> ^q;  this->client_data.TryRemove(sock, q);
				this->lockedBag = true;
				List<SOCKET> ss;
				while (!client_sockets.IsEmpty)
				{
					SOCKET s;
					this->client_sockets.TryTake(s);
					if (s == sock) break;
					else ss.Add(s);
				}
				for (int i = 0; i < ss.Count; i++)
					this->client_sockets.Add(ss[i]);
				this->lockedBag = false;
			}
		};
		ref class Client : public Engine
		{
			enum class ClientStates
			{
				Update, Disconect, SendMessage
			};
			ClientStates state;
			const size_t delay_ms = 200;
			Thread^ connect_thr;
			String ^server_name;
			bool terminated = false;
			
		public:
			Client(String ^name, String ^ip, String ^port, Interface ^face)
			{
				this->name = name;
				this->ip = ip;
				this->port = port;
				this->is_server = false;
				this->state = ClientStates::Update;
				this->face = face;
			}

			virtual bool Start() override
			{
				terminated = false;
				this->is_active = true;
				Socket = gcnew WinSocket();
				buff = new char[buff_len];
				face->InfoLine("Инициализация клиентского сокета...");
				if (!Socket->InitializeSocket())
				{
					ErrorMessage(5);
					Terminate();
					return false;
				}
				Socket->Set_IP(this->ip);
				Socket->Set_Port(Convert::ToUInt16(this->port));
				face->InfoLine("Подключаемся по указаному адресу...");
				if (!Socket->Connect())
				{
					ErrorMessage(6);
					Terminate();
					return false;
				}
				face->InfoLine("Подтверждаем подключение 1...");
				ClearBuffer();
				Socket->GetData(buff, buff_len);
				if (strcmp(r_hello1, buff))
				{
					ErrorMessage(7);
					Terminate();
					return false;
				}
				Socket->SendData(r_hello2, strlen(r_hello2));
				face->InfoLine("Подтверждаем подключение 2...");
				ClearBuffer();
				Socket->GetData(buff, buff_len);
				if (strcmp(r_ok, buff))
				{
					ErrorMessage(7);
					Terminate();
					return false;
				}
				face->InfoLine("Соединение установлено. Создаем поток для приема/передачи данных...");

				// Отправляем ему свое имя
				char *name = (char*)(void*)Marshal::StringToHGlobalAnsi(this->name);
				if (!Socket->SendData(name, this->name->Length))
				{
					ErrorMessage(8);
					Terminate();
					return false;
				}
				// Принимаем имена всех учасников
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len)) 
				{
					ErrorMessage(9);
					Terminate();
					return false;
				}
				if (!strcmp(buff, r_alreadyexist)) 
				{
					ErrorMessage(10);
					Terminate();
					return false;
				}

				connect_thr = gcnew Thread(gcnew ThreadStart(this, &Client::ConnectionFunc));
				connect_thr->Start();
				face->InfoLine("Готово!");
				return true;
			}
			virtual void Terminate() override
			{
				if (this->connect_thr != nullptr && this->connect_thr->ThreadState == ThreadState::Running) this->state = ClientStates::Disconect;
				else
				{
					if (!terminated) term();
					face->Message("Вы отключились от сервера.\n");
				}
			}
			virtual void SendMessage() override
			{
				if (!this->is_active) return;
				this->state = ClientStates::SendMessage;
			}

		private:
			void ConnectionFunc()
			{
				if (!Socket->SendData(r_ok, strlen(r_ok))) { ConnectionError(-4); return; }
				bool b = true;
				while (strcmp(buff, r_end_names))
				{
					face->AddMemberName(gcnew String(buff));
					if (b) { server_name = gcnew String(buff); b = false; }
					ClearBuffer();
					if (!Socket->GetData(buff, buff_len)) { ConnectionError(-5); return; }
					if (!Socket->SendData(r_ok, strlen(r_ok))) { ConnectionError(-6); return; }
				}
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len) | strcmp(buff, r_ok)) { ConnectionError(-7); return; }
				face->Message("Вы поключились к серверу " + server_name + ".");
				// Запускаем конечный автомат связи с сервером.
				while (this->is_active)
				{
					Sleep(this->delay_ms);
					switch (this->state)
					{
					case ClientStates::Update: { if (!RequestUpdate()) return; } break;
					case ClientStates::SendMessage: { if (!RequestSendMessage()) return; } break;
					case ClientStates::Disconect: { RequestDisconect(); return; } break;
					}
				}
			}

			bool RequestUpdate()
			{
				face->Indicate();
				do
				{
					if (!Socket->SendData(r_update, strlen(r_update))) { ConnectionError(-8); return false; };
					ClearBuffer();
					if (!Socket->GetData(buff, buff_len)) { ConnectionError(-9); return false; };
				} while (!strcmp(buff, r_repeat));
				while (strcmp(buff, r_end_update))
				{
					if (buff[0] == 'N' && buff[1] == 'E' && buff[2] == 'W')
					{
						String ^s = gcnew String(buff);
						s = s->Substring(3);
						face->AddMemberName(s);
					}
					else face->MessageUser(gcnew String(buff));
					
					if (!Socket->SendData(r_ok, strlen(r_ok))) { ConnectionError(-10); return false; };
					ClearBuffer();
					if (!Socket->GetData(buff, buff_len)) { ConnectionError(-11); return false; };
				}
				ClearBuffer();
				return true;
			}
			bool RequestSendMessage()
			{
				if (!Socket->SendData(r_sendmessage, strlen(r_sendmessage))) { ConnectionError(-12); return false; };
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len)) { ConnectionError(-13); return false; };
				if (strcmp(buff, r_ok)) { ConnectionError(-14); return false; };

				String ^s = face->CurrentMessageGet();
				face->CurrentMessageClear();
				char *k = (char*)(void*)Marshal::StringToHGlobalAnsi(s);
				if (!Socket->SendData(k, s->Length)) { ConnectionError(-15); return false; };

				ClearBuffer();
				if (!Socket->GetData(buff, buff_len)) { ConnectionError(-16); return false; };
				if (strcmp(buff, r_ok)) { ConnectionError(-17); return false; };
				this->state = ClientStates::Update;
				return true;
			}
			bool RequestDisconect()
			{
				if (!Socket->SendData(r_disconnect, strlen(r_disconnect))) { ConnectionError(-18); return false; };
				ClearBuffer();
				if (!Socket->GetData(buff, buff_len)) { ConnectionError(-19); return false; };
				if (strcmp(buff, r_ok)) { ConnectionError(-20); return false; };
				face->Message("Вы отключились от сервера.\n");
				term();
				return true;
			}

			void ConnectionError(int errnum)
			{
				this->face->Message("Проблемы с подключением с сервером, соединение разорвано.");
				String ^reason;
				switch (errnum)
				{
				case -1:  { reason = "Не удалось отправить серверу свое имя"; } break;
				case -2:  { reason = "Не удалось принять ответ сервера на ваше имя"; } break;
				case -3:  { reason = "Ваше имя уже используется в чате"; } break;
				case -4:  { reason = "Не удалось отправить подтверждение на прием первого ответа сервера"; } break;
				case -5:  { reason = "Не удалось принять очередное имя участника"; } break;
				case -6:  { reason = "Не удалось отправить подтверждение приема очередного имени участника"; } break;
				case -7:  { reason = "Не удалось принять последний запрос сервера перед началом цикла запросов"; } break;
				case -8:  { reason = "Не удалось отправить запрос на обновление"; } break;
				case -9:  { reason = "Не удалось принять первый ответ на запрос на обновление"; } break;
				case -10: { reason = "Не удалось отправить подтверждение приема очередного ответа на запрос"; } break;
				case -11: { reason = "Не удалось принять очередной ответ на запрос на обновление"; } break;
				case -12: { reason = "Не удалось отправить запрос на передачу сообщения"; } break;
				case -13: { reason = "Не удалось принять подтверждение на передачу сообщения"; } break;
				case -14: { reason = "Не было получено подтверждение на передачу сообщения"; } break;
				case -15: { reason = "Не удалось передать сообщение серверу"; } break;
				case -16: { reason = "Не удалось принять подтверждение приема сообщения"; } break;
				case -17: { reason = "Вместо подтверждения приема сообщения принят некорректный запрос"; } break;
				case -18: { reason = "Не удалось отправить запрос на отключение соединения"; } break;
				case -19: { reason = "Не удалось получить подтверждение на отключение соединения"; } break;
				case -20: { reason = "Вместо подтверждения на отключение принят некорректный запрос"; } break;
				}
				this->face->Message("Причина " + Convert::ToString(errnum) + ": " + reason + ".");
				term();
			}
			void term()
			{
				terminated = true;
				Socket->~WinSocket();
				delete[] buff;
				is_active = false;
				face->ClearMemberBox();
				face->InfoLine("Введите IP и порт сервера для подключения.");
				face->Deindicate();
			}
		};

		Engine ^engine;
		Interface ^face;
		void StartServer()
		{
			engine = gcnew Server(tb_name->Text, tb_ip->Text, tb_port->Text, face);
		}
		void StartClient()
		{
			engine = gcnew Client(tb_name->Text, tb_ip->Text, tb_port->Text, face);
		}

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
	private: System::Windows::Forms::Button^  b_cancel;
	private: System::Windows::Forms::Label^  label_indicator;
	private: System::Windows::Forms::ListBox^  lb_members;
	private: System::Windows::Forms::Label^  label_members;
	private: System::Windows::Forms::Panel^  panel_top;
	private: System::Windows::Forms::TextBox^  tb_name;
	private: System::Windows::Forms::Label^  label_name;
	private: System::Windows::Forms::Button^  b_ok_name;
	private: System::Windows::Forms::Button^  b_create_room;
	private: System::Windows::Forms::Button^  b_connet_room;
	private: System::Windows::Forms::Panel^  panel_main;
	private: System::Windows::Forms::RichTextBox^  rbt_my_msg;
	private: System::Windows::Forms::Button^  b_send;
	private: System::Windows::Forms::Label^  label_info;
	private: System::Windows::Forms::RichTextBox^  rtb_all_msg;
	private: System::Windows::Forms::Panel^  panel_info;
	private: System::Windows::Forms::Label^  label_server_info;
	private: System::Windows::Forms::Label^  label_port;
	private: System::Windows::Forms::Label^  label_ip;
	private: System::Windows::Forms::TextBox^  tb_ip;
	private: System::Windows::Forms::TextBox^  tb_port;
	private: System::Windows::Forms::Button^  b_connect;
	private: System::ComponentModel::Container ^components;
	private: System::Data::OleDb::OleDbCommand^  oleDbSelectCommand1;
	private: System::Data::OleDb::OleDbConnection^  oleDbConnection1;
	private: System::Data::OleDb::OleDbCommand^  oleDbInsertCommand1;
	private: System::Data::OleDb::OleDbCommand^  oleDbUpdateCommand1;
	private: System::Data::OleDb::OleDbCommand^  oleDbDeleteCommand1;
	private: System::Data::OleDb::OleDbDataAdapter^  oleDbDataAdapter1;
	private: System::Data::DataSet^  dataSet;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->panel_top = (gcnew System::Windows::Forms::Panel());
			this->b_create_room = (gcnew System::Windows::Forms::Button());
			this->b_connet_room = (gcnew System::Windows::Forms::Button());
			this->b_ok_name = (gcnew System::Windows::Forms::Button());
			this->tb_name = (gcnew System::Windows::Forms::TextBox());
			this->label_name = (gcnew System::Windows::Forms::Label());
			this->panel_main = (gcnew System::Windows::Forms::Panel());
			this->lb_members = (gcnew System::Windows::Forms::ListBox());
			this->label_members = (gcnew System::Windows::Forms::Label());
			this->rtb_all_msg = (gcnew System::Windows::Forms::RichTextBox());
			this->b_send = (gcnew System::Windows::Forms::Button());
			this->rbt_my_msg = (gcnew System::Windows::Forms::RichTextBox());
			this->label_info = (gcnew System::Windows::Forms::Label());
			this->panel_info = (gcnew System::Windows::Forms::Panel());
			this->b_cancel = (gcnew System::Windows::Forms::Button());
			this->b_connect = (gcnew System::Windows::Forms::Button());
			this->tb_ip = (gcnew System::Windows::Forms::TextBox());
			this->tb_port = (gcnew System::Windows::Forms::TextBox());
			this->label_port = (gcnew System::Windows::Forms::Label());
			this->label_ip = (gcnew System::Windows::Forms::Label());
			this->label_server_info = (gcnew System::Windows::Forms::Label());
			this->label_indicator = (gcnew System::Windows::Forms::Label());
			this->oleDbSelectCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbConnection1 = (gcnew System::Data::OleDb::OleDbConnection());
			this->oleDbInsertCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbUpdateCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbDeleteCommand1 = (gcnew System::Data::OleDb::OleDbCommand());
			this->oleDbDataAdapter1 = (gcnew System::Data::OleDb::OleDbDataAdapter());
			this->dataSet = (gcnew System::Data::DataSet());
			this->panel_top->SuspendLayout();
			this->panel_main->SuspendLayout();
			this->panel_info->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataSet))->BeginInit();
			this->SuspendLayout();
			// 
			// panel_top
			// 
			this->panel_top->Controls->Add(this->b_create_room);
			this->panel_top->Controls->Add(this->b_connet_room);
			this->panel_top->Location = System::Drawing::Point(0, 38);
			this->panel_top->Name = L"panel_top";
			this->panel_top->Size = System::Drawing::Size(354, 76);
			this->panel_top->TabIndex = 0;
			// 
			// b_create_room
			// 
			this->b_create_room->Enabled = false;
			this->b_create_room->Location = System::Drawing::Point(12, 3);
			this->b_create_room->Name = L"b_create_room";
			this->b_create_room->Size = System::Drawing::Size(168, 61);
			this->b_create_room->TabIndex = 4;
			this->b_create_room->Text = L"Создать комнату";
			this->b_create_room->UseVisualStyleBackColor = true;
			this->b_create_room->Click += gcnew System::EventHandler(this, &MyForm::b_create_room_Click);
			// 
			// b_connet_room
			// 
			this->b_connet_room->Enabled = false;
			this->b_connet_room->Location = System::Drawing::Point(186, 3);
			this->b_connet_room->Name = L"b_connet_room";
			this->b_connet_room->Size = System::Drawing::Size(163, 61);
			this->b_connet_room->TabIndex = 3;
			this->b_connet_room->Text = L"Подключится к комнате";
			this->b_connet_room->UseVisualStyleBackColor = true;
			this->b_connet_room->Click += gcnew System::EventHandler(this, &MyForm::b_connet_room_Click);
			// 
			// b_ok_name
			// 
			this->b_ok_name->Location = System::Drawing::Point(298, 9);
			this->b_ok_name->Name = L"b_ok_name";
			this->b_ok_name->Size = System::Drawing::Size(56, 23);
			this->b_ok_name->TabIndex = 2;
			this->b_ok_name->Text = L"OK";
			this->b_ok_name->UseVisualStyleBackColor = true;
			this->b_ok_name->Click += gcnew System::EventHandler(this, &MyForm::b_ok_name_Click);
			// 
			// tb_name
			// 
			this->tb_name->Location = System::Drawing::Point(110, 9);
			this->tb_name->Name = L"tb_name";
			this->tb_name->Size = System::Drawing::Size(182, 22);
			this->tb_name->TabIndex = 1;
			this->tb_name->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_name_KeyPress);
			// 
			// label_name
			// 
			this->label_name->AutoSize = true;
			this->label_name->Location = System::Drawing::Point(8, 9);
			this->label_name->Name = L"label_name";
			this->label_name->Size = System::Drawing::Size(96, 17);
			this->label_name->TabIndex = 0;
			this->label_name->Text = L"Введите имя:";
			// 
			// panel_main
			// 
			this->panel_main->Controls->Add(this->lb_members);
			this->panel_main->Controls->Add(this->label_members);
			this->panel_main->Controls->Add(this->rtb_all_msg);
			this->panel_main->Controls->Add(this->b_send);
			this->panel_main->Controls->Add(this->rbt_my_msg);
			this->panel_main->Enabled = false;
			this->panel_main->Location = System::Drawing::Point(0, 110);
			this->panel_main->Name = L"panel_main";
			this->panel_main->Size = System::Drawing::Size(885, 512);
			this->panel_main->TabIndex = 1;
			// 
			// lb_members
			// 
			this->lb_members->FormattingEnabled = true;
			this->lb_members->ItemHeight = 16;
			this->lb_members->Location = System::Drawing::Point(673, 31);
			this->lb_members->Name = L"lb_members";
			this->lb_members->Size = System::Drawing::Size(199, 420);
			this->lb_members->TabIndex = 6;
			// 
			// label_members
			// 
			this->label_members->AutoSize = true;
			this->label_members->Location = System::Drawing::Point(670, 11);
			this->label_members->Name = L"label_members";
			this->label_members->Size = System::Drawing::Size(82, 17);
			this->label_members->TabIndex = 5;
			this->label_members->Text = L"Участники:";
			// 
			// rtb_all_msg
			// 
			this->rtb_all_msg->Location = System::Drawing::Point(11, 3);
			this->rtb_all_msg->Name = L"rtb_all_msg";
			this->rtb_all_msg->ReadOnly = true;
			this->rtb_all_msg->Size = System::Drawing::Size(652, 449);
			this->rtb_all_msg->TabIndex = 4;
			this->rtb_all_msg->Text = L"";
			// 
			// b_send
			// 
			this->b_send->Location = System::Drawing::Point(673, 458);
			this->b_send->Name = L"b_send";
			this->b_send->Size = System::Drawing::Size(199, 53);
			this->b_send->TabIndex = 2;
			this->b_send->Text = L"Отправить";
			this->b_send->UseVisualStyleBackColor = true;
			this->b_send->Click += gcnew System::EventHandler(this, &MyForm::b_send_Click);
			// 
			// rbt_my_msg
			// 
			this->rbt_my_msg->Location = System::Drawing::Point(12, 458);
			this->rbt_my_msg->Name = L"rbt_my_msg";
			this->rbt_my_msg->Size = System::Drawing::Size(651, 53);
			this->rbt_my_msg->TabIndex = 0;
			this->rbt_my_msg->Text = L"";
			// 
			// label_info
			// 
			this->label_info->AutoSize = true;
			this->label_info->Location = System::Drawing::Point(8, 625);
			this->label_info->Name = L"label_info";
			this->label_info->Size = System::Drawing::Size(222, 17);
			this->label_info->TabIndex = 3;
			this->label_info->Text = L"Введите ваше имя или никнейм.";
			// 
			// panel_info
			// 
			this->panel_info->Controls->Add(this->b_cancel);
			this->panel_info->Controls->Add(this->b_connect);
			this->panel_info->Controls->Add(this->tb_ip);
			this->panel_info->Controls->Add(this->tb_port);
			this->panel_info->Controls->Add(this->label_port);
			this->panel_info->Controls->Add(this->label_ip);
			this->panel_info->Controls->Add(this->label_server_info);
			this->panel_info->Enabled = false;
			this->panel_info->Location = System::Drawing::Point(355, -2);
			this->panel_info->Name = L"panel_info";
			this->panel_info->Size = System::Drawing::Size(530, 116);
			this->panel_info->TabIndex = 2;
			// 
			// b_cancel
			// 
			this->b_cancel->Location = System::Drawing::Point(27, 86);
			this->b_cancel->Name = L"b_cancel";
			this->b_cancel->Size = System::Drawing::Size(31, 23);
			this->b_cancel->TabIndex = 6;
			this->b_cancel->Text = L"X";
			this->b_cancel->UseVisualStyleBackColor = true;
			this->b_cancel->Visible = false;
			this->b_cancel->Click += gcnew System::EventHandler(this, &MyForm::b_cancel_Click);
			// 
			// b_connect
			// 
			this->b_connect->Location = System::Drawing::Point(64, 85);
			this->b_connect->Name = L"b_connect";
			this->b_connect->Size = System::Drawing::Size(116, 24);
			this->b_connect->TabIndex = 5;
			this->b_connect->Text = L"Connect";
			this->b_connect->UseVisualStyleBackColor = true;
			this->b_connect->Visible = false;
			this->b_connect->Click += gcnew System::EventHandler(this, &MyForm::b_connect_Click);
			// 
			// tb_ip
			// 
			this->tb_ip->Location = System::Drawing::Point(64, 29);
			this->tb_ip->Name = L"tb_ip";
			this->tb_ip->ReadOnly = true;
			this->tb_ip->Size = System::Drawing::Size(116, 22);
			this->tb_ip->TabIndex = 4;
			this->tb_ip->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_ip_KeyPress);
			// 
			// tb_port
			// 
			this->tb_port->Location = System::Drawing::Point(64, 57);
			this->tb_port->Name = L"tb_port";
			this->tb_port->ReadOnly = true;
			this->tb_port->Size = System::Drawing::Size(116, 22);
			this->tb_port->TabIndex = 3;
			this->tb_port->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_port_KeyPress);
			// 
			// label_port
			// 
			this->label_port->AutoSize = true;
			this->label_port->Location = System::Drawing::Point(20, 60);
			this->label_port->Name = L"label_port";
			this->label_port->Size = System::Drawing::Size(38, 17);
			this->label_port->TabIndex = 2;
			this->label_port->Text = L"Port:";
			// 
			// label_ip
			// 
			this->label_ip->AutoSize = true;
			this->label_ip->Location = System::Drawing::Point(34, 34);
			this->label_ip->Name = L"label_ip";
			this->label_ip->Size = System::Drawing::Size(24, 17);
			this->label_ip->TabIndex = 1;
			this->label_ip->Text = L"IP:";
			// 
			// label_server_info
			// 
			this->label_server_info->AutoSize = true;
			this->label_server_info->Location = System::Drawing::Point(7, 11);
			this->label_server_info->Name = L"label_server_info";
			this->label_server_info->Size = System::Drawing::Size(193, 17);
			this->label_server_info->TabIndex = 0;
			this->label_server_info->Text = L"Информация о соединении.";
			// 
			// label_indicator
			// 
			this->label_indicator->AutoSize = true;
			this->label_indicator->Location = System::Drawing::Point(810, 625);
			this->label_indicator->Name = L"label_indicator";
			this->label_indicator->Size = System::Drawing::Size(62, 17);
			this->label_indicator->TabIndex = 4;
			this->label_indicator->Text = L"Indicator";
			// 
			// oleDbSelectCommand1
			// 
			this->oleDbSelectCommand1->CommandText = L"SELECT datatable.*\r\nFROM     datatable";
			this->oleDbSelectCommand1->Connection = this->oleDbConnection1;
			// 
			// oleDbConnection1
			// 
			this->oleDbConnection1->ConnectionString = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\database.mdb";
			// 
			// oleDbInsertCommand1
			// 
			this->oleDbInsertCommand1->CommandText = L"INSERT INTO `datatable` (`Time`, `Sender`, `Message`) VALUES (\?, \?, \?)";
			this->oleDbInsertCommand1->Connection = this->oleDbConnection1;
			this->oleDbInsertCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(3) {
				(gcnew System::Data::OleDb::OleDbParameter(L"Time",
					System::Data::OleDb::OleDbType::Date, 0, L"Time")), (gcnew System::Data::OleDb::OleDbParameter(L"Sender", System::Data::OleDb::OleDbType::VarWChar,
						0, L"Sender")), (gcnew System::Data::OleDb::OleDbParameter(L"Message", System::Data::OleDb::OleDbType::LongVarWChar, 0, L"Message"))
			});
			// 
			// oleDbUpdateCommand1
			// 
			this->oleDbUpdateCommand1->CommandText = L"UPDATE `datatable` SET `Time` = \?, `Sender` = \?, `Message` = \? WHERE ((`Counter` "
				L"= \?) AND ((\? = 1 AND `Time` IS NULL) OR (`Time` = \?)) AND ((\? = 1 AND `Sender` I"
				L"S NULL) OR (`Sender` = \?)))";
			this->oleDbUpdateCommand1->Connection = this->oleDbConnection1;
			this->oleDbUpdateCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(8) {
				(gcnew System::Data::OleDb::OleDbParameter(L"Time",
					System::Data::OleDb::OleDbType::Date, 0, L"Time")), (gcnew System::Data::OleDb::OleDbParameter(L"Sender", System::Data::OleDb::OleDbType::VarWChar,
						0, L"Sender")), (gcnew System::Data::OleDb::OleDbParameter(L"Message", System::Data::OleDb::OleDbType::LongVarWChar, 0, L"Message")),
						(gcnew System::Data::OleDb::OleDbParameter(L"Original_Counter", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
							false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Counter", System::Data::DataRowVersion::Original, nullptr)),
							(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Time", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
								static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Time", System::Data::DataRowVersion::Original, true, nullptr)),
								(gcnew System::Data::OleDb::OleDbParameter(L"Original_Time", System::Data::OleDb::OleDbType::Date, 0, System::Data::ParameterDirection::Input,
									false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Time", System::Data::DataRowVersion::Original, nullptr)),
									(gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Sender", System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input,
										static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Sender", System::Data::DataRowVersion::Original, true, nullptr)),
										(gcnew System::Data::OleDb::OleDbParameter(L"Original_Sender", System::Data::OleDb::OleDbType::VarWChar, 0, System::Data::ParameterDirection::Input,
											false, static_cast<System::Byte>(0), static_cast<System::Byte>(0), L"Sender", System::Data::DataRowVersion::Original, nullptr))
			});
			// 
			// oleDbDeleteCommand1
			// 
			this->oleDbDeleteCommand1->CommandText = L"DELETE FROM `datatable` WHERE ((`Counter` = \?) AND ((\? = 1 AND `Time` IS NULL) OR"
				L" (`Time` = \?)) AND ((\? = 1 AND `Sender` IS NULL) OR (`Sender` = \?)))";
			this->oleDbDeleteCommand1->Connection = this->oleDbConnection1;
			this->oleDbDeleteCommand1->Parameters->AddRange(gcnew cli::array< System::Data::OleDb::OleDbParameter^  >(5) {
				(gcnew System::Data::OleDb::OleDbParameter(L"Original_Counter",
					System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
					static_cast<System::Byte>(0), L"Counter", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Time",
						System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
						L"Time", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Time",
							System::Data::OleDb::OleDbType::Date, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
							L"Time", System::Data::DataRowVersion::Original, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"IsNull_Sender",
								System::Data::OleDb::OleDbType::Integer, 0, System::Data::ParameterDirection::Input, static_cast<System::Byte>(0), static_cast<System::Byte>(0),
								L"Sender", System::Data::DataRowVersion::Original, true, nullptr)), (gcnew System::Data::OleDb::OleDbParameter(L"Original_Sender",
									System::Data::OleDb::OleDbType::VarWChar, 0, System::Data::ParameterDirection::Input, false, static_cast<System::Byte>(0),
									static_cast<System::Byte>(0), L"Sender", System::Data::DataRowVersion::Original, nullptr))
			});
			// 
			// oleDbDataAdapter1
			// 
			this->oleDbDataAdapter1->DeleteCommand = this->oleDbDeleteCommand1;
			this->oleDbDataAdapter1->InsertCommand = this->oleDbInsertCommand1;
			this->oleDbDataAdapter1->SelectCommand = this->oleDbSelectCommand1;
			cli::array< System::Data::Common::DataColumnMapping^ >^ __mcTemp__1 = gcnew cli::array< System::Data::Common::DataColumnMapping^  >(4) {
				(gcnew System::Data::Common::DataColumnMapping(L"Counter",
					L"Counter")), (gcnew System::Data::Common::DataColumnMapping(L"Time", L"Time")), (gcnew System::Data::Common::DataColumnMapping(L"Sender",
						L"Sender")), (gcnew System::Data::Common::DataColumnMapping(L"Message", L"Message"))
			};
			this->oleDbDataAdapter1->TableMappings->AddRange(gcnew cli::array< System::Data::Common::DataTableMapping^  >(1) {
				(gcnew System::Data::Common::DataTableMapping(L"Table",
					L"datatable", __mcTemp__1))
			});
			this->oleDbDataAdapter1->UpdateCommand = this->oleDbUpdateCommand1;
			// 
			// dataSet
			// 
			this->dataSet->DataSetName = L"MaaDataSet";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(884, 646);
			this->Controls->Add(this->label_indicator);
			this->Controls->Add(this->panel_info);
			this->Controls->Add(this->label_info);
			this->Controls->Add(this->b_ok_name);
			this->Controls->Add(this->tb_name);
			this->Controls->Add(this->panel_main);
			this->Controls->Add(this->panel_top);
			this->Controls->Add(this->label_name);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->ShowIcon = false;
			this->Text = L"Сетевой чат";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &MyForm::MyForm_Shown);
			this->panel_top->ResumeLayout(false);
			this->panel_main->ResumeLayout(false);
			this->panel_main->PerformLayout();
			this->panel_info->ResumeLayout(false);
			this->panel_info->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataSet))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	void AutoSetIP()
	{
		WinSocket ^sock = gcnew WinSocket();
		if (!sock->InitializeSocket()) return;
		sock->Set_IPAuto();
		sock->Set_PortAuto();
		if (!sock->Bind()) return;
		this->tb_ip->Text = sock->GetIP();
		sock->~WinSocket();
	}

	private: System::Void MyForm_Shown(System::Object^  sender, System::EventArgs^  e) 
	{
		if (!WinSocket::InitializeLibrary())
		{
			Engine::ErrorMessage(0);
			Application::Exit();
		}
		face = gcnew Interface(label_info, label_indicator, lb_members, rtb_all_msg, rbt_my_msg, dataSet, oleDbDataAdapter1);
		engine = gcnew Engine(face);
		dataSet->Reset();
		oleDbDataAdapter1->Fill(dataSet);
	}
	private: System::Void MyForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) 
	{
		if (engine->IsActive()) engine->Terminate();
		if (!WinSocket::CloseLibrary())
		{
			Engine::ErrorMessage(1);
			Application::Exit();
		}
	}

	private: System::Void tb_name_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (!((e->KeyChar > 47 && e->KeyChar < 59) || (e->KeyChar >= 65 && e->KeyChar <= 122) || e->KeyChar == 8) || e->KeyChar == 92)
			e->Handled = true;
	}
	private: System::Void b_ok_name_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb_name->Text->Length == 0) return;
		b_ok_name->Enabled = false;
		tb_name->ReadOnly = true;
		b_create_room->Enabled = true;
		b_connet_room->Enabled = true;
		label_info->Text = "Выберите режим работы программы.";
	}
	private: System::Void b_create_room_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		StartServer();
		rtb_all_msg->Text = "";
		if (!engine->Start()) return;

		tb_ip->Text = engine->GetIP();
		tb_port->Text = engine->GetPort();
		panel_info->Enabled = true;
		panel_top->Enabled = false;
		panel_main->Enabled = true;
		b_connect->Text = "Disconnect";
		b_connect->Visible = true;
		b_cancel->Visible = true;
		b_cancel->Enabled = false;
	}
	private: System::Void b_connet_room_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		AutoSetIP();
		panel_top->Enabled = false;
		panel_info->Enabled = true;
		b_connect->Visible = true;
		b_cancel->Visible = true;
		tb_ip->ReadOnly = false;
		tb_port->ReadOnly = false;
		label_info->Text = "Введите IP и порт сервера для подключения.";
		rtb_all_msg->Text = "";
	}
	
	private: System::Void tb_ip_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8 && e->KeyChar != 46)
			e->Handled = true;
	}
	private: System::Void tb_port_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8) || (tb_port->Text->Length == 5 && e->KeyChar != 8))
			e->Handled = true;
	}
	private: System::Void b_connect_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if (b_connect->Text == "Connect")
		{
			rtb_all_msg->Text = "";
			if (tb_ip->Text->Length == 0 || tb_port->Text->Length == 0)
			{
				Engine::ErrorMessage(8);
				return;
			}
			StartClient();
			if (!engine->Start()) return;
			b_connect->Text = "Disconnect";
			panel_top->Enabled = false;
			panel_main->Enabled = true;
			tb_ip->ReadOnly = true;
			tb_port->ReadOnly = true;
			b_cancel->Enabled = false;
		}
		else
		{
			label_indicator->Text = "Indicator";
			engine->Terminate();
			b_connect->Text = "Connect";
			tb_ip->Text = "";
			tb_port->Text = "";
			panel_top->Enabled = true;
			panel_main->Enabled = false;
			panel_info->Enabled = false;
			b_cancel->Enabled = true;
			label_info->Text = "Выберите режим работы программы.";
		}
	}
	private: System::Void b_cancel_Click(System::Object^  sender, System::EventArgs^  e)
	{
		b_cancel->Visible = false;
		b_connect->Visible = false;
		panel_info->Enabled = false;
		panel_top->Enabled = true;
	}

	private: System::Void b_send_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if (rbt_my_msg->Text != "")
			engine->SendMessage();
	}
};
}