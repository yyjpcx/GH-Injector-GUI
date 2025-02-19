#include "pch.h"

#include "ShortCut.h"

HRESULT CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszDesc, LPCWSTR args)
{
	HRESULT			hres	= S_OK;
	IShellLinkW *	psl		= nullptr;

	hres = CoInitialize(NULL);
	if (FAILED(hres))
	{
		// Has to be called for failed calls to CoInitialize too.
		CoUninitialize(); 

		return hres;
	}

	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile * ppf = nullptr;

		// Set the path to the shortcut target and add the description. 
		psl->SetPath(lpszPathObj);
		psl->SetDescription(lpszDesc);

		psl->SetArguments(args);
		psl->SetWorkingDirectory(lpszDesc);

		// Query IShellLink for the IPersistFile interface, used for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

		if (SUCCEEDED(hres))
		{
			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(lpszPathLink, TRUE);
			ppf->Release();
		}

		psl->Release();
	}

	CoUninitialize();

	return hres;
}

HRESULT CreateLinkWrapper(QString linkName, QString linkArgument)
{
	QString desPath = QCoreApplication::applicationDirPath() + "\\";
	QString InjectorExe = QCoreApplication::applicationFilePath();

	desPath.replace("/", "\\");
	InjectorExe.replace("/", "\\");

	QString completeLinkName = desPath + linkName + QString(".lnk");

	wchar_t str1[1024] = { 0 };
	InjectorExe.toWCharArray(str1);

	wchar_t str3[1024] = { 0 };
	desPath.toWCharArray(str3);

	wchar_t str4[1024] = { 0 };
	linkArgument.toWCharArray(str4);

	return CreateLink(str1, completeLinkName.toStdWString().c_str(), str3, str4);
}