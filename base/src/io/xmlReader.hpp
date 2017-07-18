#pragma once

#include "xml.h"

#include <iostream>

class XmlReader {
public:
	
	void Run()
	{
		CComPtr<IXmlReader> pReader;
		if (FAILED(CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&pReader), 0))) {
			MessageBox(NULL, _T("CreateXmlReader���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
			return;
		}

		// XML�t�@�C���p�X�쐬
		//TCHAR xml[MAX_PATH];
		//GetModuleFileName(NULL, xml, sizeof(xml) / sizeof(TCHAR));
		//PathRemoveFileSpec(xml);
		//PathAppend(xml, _T("sample.xml"));
		char xml[] = "sample.xml"; //OK

		{
			// �t�@�C���X�g���[���쐬
			CComPtr<IStream> pStream;
			if (FAILED(SHCreateStreamOnFile(xml, STGM_READ, &pStream))) {
				MessageBox(NULL, _T("SHCreateStreamOnFile���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
				return;
			}

			if (FAILED(pReader->SetInput(pStream))) {
				MessageBox(NULL, _T("SetInput���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
				return;
			}

			//----- UTF-8 Setting -----
			//Open in UTF-8 Mode <-- �f�t�H���g�G���R�[�h�̂ŁA�Ȃ��Ă��ǂ�
			//{
			//	CComPtr<IXmlReaderInput> pInput;
			//	if (FAILED(CreateXmlReaderInputWithEncodingName(pStream, NULL, L"UTF-8", FALSE, NULL, &pInput))) {
			//		MessageBox(NULL, _T("CreateXmlReaderInputWithEncodingName���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
			//		return;
			//	}
			//	if (FAILED(pReader->SetInput(pInput))) {
			//		MessageBox(NULL, _T("SetInput���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
			//		return;
			//	}
			//}
			//----- End UTF-8 Setting -----

			//----- Shift_JIS Setting -----
			//{
				//CComPtr<IMultiLanguage2> pLang;
				//if (FAILED(pLang.CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER))) {
				//	MessageBox(NULL, _T("CoCreateInstance���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
				//	return;
				//}
				//
				//if (FAILED(pReader->SetProperty(XmlReaderProperty_MultiLanguage,
				//	reinterpret_cast<LONG_PTR>(pLang.p)))) {
				//	MessageBox(NULL, _T("SetProperty���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
				//	return;
				//}
			//}
			//----- End Shift_JIS Setting
		}


		//Long Pointer Wide String (Unicode)
		//�R���p�C���G���[������߂ɁAnullptr������������
		LPCWSTR pwszLocalName = nullptr;
		LPCWSTR pwszValue = nullptr;

		//----- Sample wstring
		////L"Text" : L��safix --> wchar���w��
		//std::wstring ws(L"Text");
		//std::wcout << ws.c_str() << std::endl;
		//----- End Sample wstring

		//----- Sample Convert wchar* -> char*
		//char res[255];
		//size_t i;
		//wcstombs_s(&i, res, 255, pwszLocalName, 255);
		//std::cout << res << std::endl;
		////���ӁF���{��̓A�E�g�I�I
		//----- End Sample Convet wchar* -> char*

		//----- Node Type -----
		//XmlNodeType_None					|	�Ȃ�
		//XmlNodeType_Element				|	�v�f�̊J�n
		//XmlNodeType_Attribute				|	����
		//XmlNodeType_Text					|	�e�L�X�g
		//XmlNodeType_CDATA					|	CDATA�Z�N�V����
		//XmlNodeType_ProcessingInstruction	|	XML�v���Z�b�T��������
		//XmlNodeType_Comment				|	�R�����g
		//XmlNodeType_DocumentType			|	�h�L�������g�̌^�錾�iDTD�j
		//XmlNodeType_Whitespace			|	��
		//XmlNodeType_EndElement			|	�v�f�̏I��
		//XmlNodeType_XmlDeclaration		|	XML�錾
		XmlNodeType nodeType;

		HRESULT hr;
		while (pReader->Read(&nodeType) == S_OK) {
			switch (nodeType) {
			case XmlNodeType_Element:
				if (FAILED(pReader->GetLocalName(&pwszLocalName, NULL))) {
					MessageBox(NULL, _T("GetLocalName���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
					return;
				}

				hr = pReader->MoveToFirstAttribute();
				
				// ����������
				if (hr == S_FALSE) {
					break; //break switch
				}
				if (hr != S_OK) {
					MessageBox(NULL, _T("MoveToFirstAttribute���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
					return;
				}

				do {
					LPCWSTR pwszAttributeName;
					LPCWSTR pwszAttributeValue;
					if (FAILED(pReader->GetLocalName(&pwszAttributeName, NULL))) {
						MessageBox(NULL, _T("GetLocalName���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
						return;
					}
					if (FAILED(pReader->GetValue(&pwszAttributeValue, NULL))) {
						MessageBox(NULL, _T("GetValue���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
						return;
					}
					if (lstrcmpW(pwszAttributeName, L"name") == 0) {
						std::wstring ws(pwszAttributeValue);
						std::wcout << ws.c_str() << std::endl;
					}
				} while (pReader->MoveToNextAttribute() == S_OK);


				break;
			case XmlNodeType_Text:
				if (FAILED(pReader->GetValue(&pwszValue, NULL))) {
					MessageBox(NULL, _T("GetValue���s"), _T("�x��"), MB_OK | MB_ICONWARNING);
					return;
				}

				if (lstrcmpW(pwszLocalName, L"type") == 0) {
					std::wstring ws(pwszValue);
					std::wcout << ws.c_str() << std::endl;
				}
				else if (lstrcmpW(pwszLocalName, L"name") == 0) {
					std::wstring ws(pwszValue);
					std::wcout << ws.c_str() << std::endl;
				}
				break;
			}
		}
	}
};