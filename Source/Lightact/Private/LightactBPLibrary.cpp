// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LightactBPLibrary.h"
#include "Lightact.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include <Windows.h>

ULightactBPLibrary::ULightactBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

/* Reads a JSON string from shared memory file and outputs a map of strings to strings. */
void ULightactBPLibrary::BuildMap(const FString HandleName, const int HandleSize, TMap<FString, FString>& resultMap, bool& success) {

	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = OpenFileMapping(
		FILE_MAP_READ, // read access
		FALSE, // do not inherit the name
		(LPCWSTR)*HandleName);

	if (hMapFile != NULL) {

		pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
			FILE_MAP_READ, // read permission
			0, // offset hi
			0, // offset lo
			HandleSize);

		if (pBuf != NULL) {

			std::string mConvert((char*)pBuf);
			UnmapViewOfFile(pBuf);

			rapidjson::Document document;
			if (!document.Parse(mConvert.c_str()).HasParseError()) {

				for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
					itr != document.MemberEnd(); ++itr) {
					resultMap.Add(itr->name.GetString(), itr->value.GetString());
				}
				CloseHandle(hMapFile);
				success = true;

			} else {
				resultMap.Add("Error", "Has Parse Error");
				CloseHandle(hMapFile);
				success = false;
			}
		} else {
			resultMap.Add("Error", "Could not map view of file");
			CloseHandle(hMapFile);
			success = false;
		}
	} else {
		resultMap.Add("Error", "Could not open file mapping object");
		success = false;
	}
}

/* Splits string by delimiters and returns a composed 3-D space vector. */
void ULightactBPLibrary::StringExplode(const FString InputString, const FString Delimiters, FVector& Vector) {

	// add default values
	TArray<float> OutConverts;
	OutConverts.Add(0.f);
	OutConverts.Add(0.f);
	OutConverts.Add(0.f);

	TArray<FString> OutSplits;
	int len = InputString.ParseIntoArray(OutSplits, Delimiters.GetCharArray().GetData(), true);

	for (int i = 0; i < min(len, 3); i++) {
		OutConverts[i] = FCString::Atof(OutSplits[i].GetCharArray().GetData());
	}
		
	Vector = FVector(OutConverts[0], OutConverts[1], OutConverts[2]);

}
