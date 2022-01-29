// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#include "TrickyAutomationHelper.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopedSlowTask.h"
#include "Misc/DateTime.h"


FString TrickyAutomationHelper::GetPluginLogDir()
{
	return FPaths::ProjectLogDir() + "TrickyAutomation/";
}

void TrickyAutomationHelper::GenerateFileName(FString& Name)
{
	FString Date = "";
	GetDate(Date);
	Name.Append("_" + Date);
}

void TrickyAutomationHelper::SaveToLogFile(const FString& Message, const FString& FileName)
{
	FString Date;
	GetDate(Date);
	const FString LogMessage = Message == ""
		                           ? FString("\n")
		                           : FString::Printf(TEXT("%s | %s\n"), *Date, *Message);
	const FString LogFileName = "Log_" + FileName + ".txt";
	const FString FilePath = GetPluginLogDir() + LogFileName;
	FFileHelper::SaveStringToFile(LogMessage,
	                              *FilePath,
	                              FFileHelper::EEncodingOptions::AutoDetect,
	                              &IFileManager::Get(),
	                              FILEWRITE_Append);
}

void TrickyAutomationHelper::CreateLogFile(FString& FileName, const FString& Message)
{
	GenerateFileName(FileName);
	SaveToLogFile(Message, FileName);
	UE_LOG(LogTrickyAutomation,
	       Display,
	       TEXT("Create log file %s in %s."),
	       *FileName,
	       *GetPluginLogDir());
}

void TrickyAutomationHelper::GetDate(FString& Date)
{
	const FDateTime CurrentTime = FDateTime::Now();
	Date = FString::Printf(TEXT("%d%02d%02d_%02d%02d%02d_%03d"),
	                       CurrentTime.GetYear(),
	                       CurrentTime.GetMonth(),
	                       CurrentTime.GetDay(),
	                       CurrentTime.GetHour(),
	                       CurrentTime.GetMinute(),
	                       CurrentTime.GetSecond(),
	                       CurrentTime.GetMillisecond());
}

void TrickyAutomationHelper::PrintMessageOnScreen(const FString& Message, const FColor& Color)
{
	if (!ensure(GEngine)) return;

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
}

bool TrickyAutomationHelper::AssetsSelectedInLibrary(const TArray<UObject*>& SelectedAssets)
{
	const bool bAreSelected = SelectedAssets.Num() > 0;

	if (!bAreSelected)
	{
		PrintMessageOnScreen("No assets selected", FColor::Red);
	}

	return bAreSelected;
}

void TrickyAutomationHelper::RenameActorInWorld(AActor* Actor, const FString& NewName)
{
	if (!Actor) return;

	Actor->Rename(*NewName);
	Actor->SetActorLabel(*NewName);
}

bool TrickyAutomationHelper::ActorsSelectedInWorld(const TArray<AActor*>& SelectedActors)
{
	const bool bSelected = SelectedActors.Num() > 0;

	if (!bSelected)
	{
		PrintMessageOnScreen("No actors are selected in the level", FColor::Red);
	}

	return bSelected;
}

bool TrickyAutomationHelper::StringIsValid(const FString& String, const FString& PropertyName)
{
	const bool bIsValid = String != "" || !String.Contains(" ") || !String.Contains("\\") || !String.Contains("/");

	if (!bIsValid)
	{
		const FString LogMessage = FString::Printf(TEXT("Invalid %s"), *PropertyName);
		PrintMessageOnScreen(LogMessage, FColor::Red);
	}

	return bIsValid;
}
