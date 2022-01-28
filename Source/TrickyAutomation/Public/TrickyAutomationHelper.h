// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#ifndef TRICKYAUTOMATIONHELPER_H
#define TRICKYAUTOMATIONHELPER_H

#pragma once

#include "CoreMinimal.h"
#include "Misc/ScopedSlowTask.h"

DECLARE_LOG_CATEGORY_CLASS(LogTrickyAutomation, Display, Error)

namespace TrickyAutomationHelper
{
	static FString GetPluginLogDir();
	static void GenerateFileName(FString& Name);
	static void SaveToLogFile(const FString& Message, const FString& FileName);
	static void CreateLogFile(FString& FileName, const FString& Message);
	static void GetDate(FString& Date);

	template <typename T>
	static void UpdateSlowTaskProgress(FScopedSlowTask& SlowTask,
	                                   const TArray<T>& Values,
	                                   const T CurrentValue)
	{
		const FString UpdatedMessage = SlowTask.DefaultMessage.ToString() + FString::Printf(
			TEXT(" %d/%d"),
			Values.IndexOfByKey(CurrentValue),
			Values.Num());
		SlowTask.EnterProgressFrame(1, FText::FromString(UpdatedMessage));
	}

	static void PrintMessageOnScreen(const FString& Message, const FColor& Color);
	static bool AssetsSelectedInLibrary(const TArray<UObject*>& SelectedAssets);
	static void RenameActorInWorld(AActor* Actor, const FString& NewName);
};

#endif
