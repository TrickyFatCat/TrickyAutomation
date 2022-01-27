// Copyright (c) 2021 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#ifndef TRICKYAUTOMATIONHELPER_H
#define TRICKYAUTOMATIONHELPER_H

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_CLASS(LogTrickyAutomation, Display, Error)

namespace TrickyAutomationHelper
{
	static FString GetPluginLogDir();
	static void GenerateFileName(FString& Name);
	static void SaveToLogFile(const FString& Message, const FString& FileName);
	static void CreateLogFile(FString& FileName, const FString& Message);
	static void GetDate(FString& Date);
	static void UpdateSlowTaskProgress(FScopedSlowTask& SlowTask,
	                                   const TArray<UObject*>& SelectedAssets,
	                                   const UObject* CurrentAsset);
	static void PrintMessageOnScreen(const FString& Message, const FColor& Color);
	static bool AssetsSelectedInLibrary(const TArray<UObject*>& SelectedAssets);
};

#endif
