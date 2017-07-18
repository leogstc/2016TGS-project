#pragma once

#include "xml.h"

#include <iostream>

class XmlReader {
public:
	
	void Run()
	{
		CComPtr<IXmlReader> pReader;
		if (FAILED(CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&pReader), 0))) {
			MessageBox(NULL, _T("CreateXmlReader失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		// XMLファイルパス作成
		//TCHAR xml[MAX_PATH];
		//GetModuleFileName(NULL, xml, sizeof(xml) / sizeof(TCHAR));
		//PathRemoveFileSpec(xml);
		//PathAppend(xml, _T("sample.xml"));
		char xml[] = "sample.xml"; //OK

		{
			// ファイルストリーム作成
			CComPtr<IStream> pStream;
			if (FAILED(SHCreateStreamOnFile(xml, STGM_READ, &pStream))) {
				MessageBox(NULL, _T("SHCreateStreamOnFile失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
				return;
			}

			if (FAILED(pReader->SetInput(pStream))) {
				MessageBox(NULL, _T("SetInput失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
				return;
			}

			//----- UTF-8 Setting -----
			//Open in UTF-8 Mode <-- デフォルトエンコードので、なくても良い
			//{
			//	CComPtr<IXmlReaderInput> pInput;
			//	if (FAILED(CreateXmlReaderInputWithEncodingName(pStream, NULL, L"UTF-8", FALSE, NULL, &pInput))) {
			//		MessageBox(NULL, _T("CreateXmlReaderInputWithEncodingName失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
			//		return;
			//	}
			//	if (FAILED(pReader->SetInput(pInput))) {
			//		MessageBox(NULL, _T("SetInput失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
			//		return;
			//	}
			//}
			//----- End UTF-8 Setting -----

			//----- Shift_JIS Setting -----
			//{
				//CComPtr<IMultiLanguage2> pLang;
				//if (FAILED(pLang.CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER))) {
				//	MessageBox(NULL, _T("CoCreateInstance失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
				//	return;
				//}
				//
				//if (FAILED(pReader->SetProperty(XmlReaderProperty_MultiLanguage,
				//	reinterpret_cast<LONG_PTR>(pLang.p)))) {
				//	MessageBox(NULL, _T("SetProperty失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
				//	return;
				//}
			//}
			//----- End Shift_JIS Setting
		}


		//Long Pointer Wide String (Unicode)
		//コンパイルエラー回避ために、nullptrを初期化する
		LPCWSTR pwszLocalName = nullptr;
		LPCWSTR pwszValue = nullptr;

		//----- Sample wstring
		////L"Text" : Lはsafix --> wcharを指定
		//std::wstring ws(L"Text");
		//std::wcout << ws.c_str() << std::endl;
		//----- End Sample wstring

		//----- Sample Convert wchar* -> char*
		//char res[255];
		//size_t i;
		//wcstombs_s(&i, res, 255, pwszLocalName, 255);
		//std::cout << res << std::endl;
		////注意：日本語はアウト！！
		//----- End Sample Convet wchar* -> char*

		//----- Node Type -----
		//XmlNodeType_None					|	なし
		//XmlNodeType_Element				|	要素の開始
		//XmlNodeType_Attribute				|	属性
		//XmlNodeType_Text					|	テキスト
		//XmlNodeType_CDATA					|	CDATAセクション
		//XmlNodeType_ProcessingInstruction	|	XMLプロセッサ処理命令
		//XmlNodeType_Comment				|	コメント
		//XmlNodeType_DocumentType			|	ドキュメントの型宣言（DTD）
		//XmlNodeType_Whitespace			|	空白
		//XmlNodeType_EndElement			|	要素の終了
		//XmlNodeType_XmlDeclaration		|	XML宣言
		XmlNodeType nodeType;

		HRESULT hr;
		while (pReader->Read(&nodeType) == S_OK) {
			switch (nodeType) {
			case XmlNodeType_Element:
				if (FAILED(pReader->GetLocalName(&pwszLocalName, NULL))) {
					MessageBox(NULL, _T("GetLocalName失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
					return;
				}

				hr = pReader->MoveToFirstAttribute();
				
				// 属性が無い
				if (hr == S_FALSE) {
					break; //break switch
				}
				if (hr != S_OK) {
					MessageBox(NULL, _T("MoveToFirstAttribute失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
					return;
				}

				do {
					LPCWSTR pwszAttributeName;
					LPCWSTR pwszAttributeValue;
					if (FAILED(pReader->GetLocalName(&pwszAttributeName, NULL))) {
						MessageBox(NULL, _T("GetLocalName失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
						return;
					}
					if (FAILED(pReader->GetValue(&pwszAttributeValue, NULL))) {
						MessageBox(NULL, _T("GetValue失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
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
					MessageBox(NULL, _T("GetValue失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
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