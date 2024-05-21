#include "stdafx.h"
#include "ExportFile.h"
#include "Converter.h"



void ExportFile::Initialize()
{
	//Tank();
	//Kachujin();
	//Tower();
	//Airplane();
}

void ExportFile::Tank()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tank/tank.fbx");

	conv->ExportMesh(L"Tank/Tank");
	conv->ExportMatrial(L"Tank/Tank", false);
	SafeDelete(conv);
}

void ExportFile::Kachujin()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Kachujin/Mesh.fbx");

	conv->ExportMesh(L"Kachujin/Mesh");
	conv->ExportMatrial(L"Kachujin/Mesh");
	SafeDelete(conv);
}

void ExportFile::Tower()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tower/Tower.fbx");

	conv->ExportMesh(L"Tower/Tower");
	conv->ExportMatrial(L"Tower/Tower");
	SafeDelete(conv);
}

void ExportFile::Airplane()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"B787/Airplane.fbx");

	conv->ExportMesh(L"B787/Airplane");
	conv->ExportMatrial(L"B787/Airplane");
	SafeDelete(conv);
}
