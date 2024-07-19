// Copyright 2020 DongGuoZheng. All Rights Reserved.


#include "InitePost.h"
#include "Json.h"
#include "Engine.h"
#include "Containers/UnrealString.h"


void UInitePost::Event_OnSuccess(FString ReturnMessage)
{
	if (OnSuccess.IsBound())
	{
		OnSuccess.Broadcast(ReturnMessage);
	}
	RemoveFromRoot();
}

void UInitePost::Event_OnFailed(FString ReturnMessage)
{
	if (OnFailed.IsBound())
	{
		OnFailed.Broadcast(ReturnMessage);
	}
	RemoveFromRoot();
}

UInitePost* UInitePost::SendActivationCode(const FString& IP, FString PhoneNum)
{


	// 创建请求工具
	UInitePost* PostTool = NewObject<UInitePost>();

		// 防止GC
	PostTool->AddToRoot();


	PostTool->StartApply(IP, PhoneNum);

	return PostTool;
}


bool UInitePost::IniteConfig( FString AccessKeyId, FString AccessSecret, FString SignName, FString TemplateCode)
{
	if (GConfig)
	{

		const TCHAR* id = *AccessKeyId;
		const TCHAR* Secret = *AccessSecret;
		const TCHAR* Name = *SignName;
		const TCHAR* Code = *TemplateCode;
					
		GConfig->SetString(TEXT("IniteConfig"), TEXT("AccessKeyId"), id, FPaths::ProjectSavedDir() / "initePost.ini");
		GConfig->SetString(TEXT("IniteConfig"), TEXT("AccessSecret"), Secret, FPaths::ProjectSavedDir() / "initePost.ini");
		GConfig->SetString(TEXT("IniteConfig"), TEXT("SignName"), Name, FPaths::ProjectSavedDir() / "initePost.ini");
		GConfig->SetString(TEXT("IniteConfig"), TEXT("TemplateCode"), Code, FPaths::ProjectSavedDir() / "initePost.ini");
		GConfig->Flush(true, FPaths::ProjectSavedDir() / "initePost.ini");
		return true;
	}
	else
	{
		return false;
	}

}

void UInitePost::StartApply(FString URl, FString PhoneNum)
{
	FString id;
	FString Secret;
	FString Name;
	FString Code;

	GConfig->GetString(TEXT("IniteConfig"), TEXT("AccessKeyId"), id, FPaths::ProjectSavedDir() / "initePost.ini");
	GConfig->GetString(TEXT("IniteConfig"), TEXT("AccessSecret"), Secret, FPaths::ProjectSavedDir() / "initePost.ini");
	GConfig->GetString(TEXT("IniteConfig"), TEXT("SignName"), Name, FPaths::ProjectSavedDir() / "initePost.ini");
	GConfig->GetString(TEXT("IniteConfig"), TEXT("TemplateCode"), Code, FPaths::ProjectSavedDir() / "initePost.ini");

	FString JsonStr;
	TSharedRef< TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&JsonStr);

	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue(TEXT("PhoneNum"), PhoneNum);
	JsonWriter->WriteValue(TEXT("AccessKeyId"), id);
	JsonWriter->WriteValue(TEXT("AccessSecret"), Secret);
	JsonWriter->WriteValue(TEXT("SignName"), Name);
	JsonWriter->WriteValue(TEXT("TemplateCode"), Code);
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();



	

	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UInitePost::OnResponseReceived);

	
	if (URl == "")
	{
		URl = "http://inite.cn";
	}
	Request->SetURL(URl + ":1234/SendM");

	Request->SetVerb("POST");

	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(JsonStr);

	Request->ProcessRequest();
}

void UInitePost::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{

	FString ReturnMessage = "No Return";

	if (!Response.IsValid())
	{
		ReturnMessage = "{\"success\":\"Error: Unable to process HTTP Request!\"}";
		Event_OnFailed(ReturnMessage);
	}
	else if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		FString TempSting;
		TempSting = Response->GetContentAsString();
		TSharedPtr<FJsonObject> rRoot;
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(TempSting);


		if (FJsonSerializer::Deserialize(Reader, rRoot))
		{
			if (rRoot->HasField(TEXT("BizId")) && "OK"==rRoot->GetStringField(TEXT("Code")))
			{
				ReturnMessage = rRoot->GetStringField(TEXT("authCode"));
				Event_OnSuccess(ReturnMessage);
			}
			else {
				Event_OnFailed(ReturnMessage);
			}
		}
		else
		{
			Event_OnFailed(ReturnMessage);
		}
		
	}
	else
	{
		ReturnMessage = FString::Printf(TEXT("{\"success\":\"HTTP Error: %d\"}"), Response->GetResponseCode());
		Event_OnFailed(ReturnMessage);
	}

}


