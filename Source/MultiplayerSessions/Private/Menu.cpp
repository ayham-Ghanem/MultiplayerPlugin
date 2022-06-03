// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "OnlineSubsystem.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"



void UMenu::MenuSetup(int32 NumberOfPublicConnection ,FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnection;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	
	UWorld* World = GetWorld();
	if (World) {

		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			

			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{

		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem) {

		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}


void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World =GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}

	}


}


bool UMenu::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}

	if (HostButton) {
		
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	}

	if (JoinButton) {

		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}


	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Session Created Successfully!")));

		}
		
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else {

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Failed to Create Session")));

		}
		HostButton->SetIsEnabled(true);
	}

}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	for(auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;

		}
	}

	if (!bWasSuccessful || SessionResults.Num() == 0) {
		
		JoinButton->SetIsEnabled(true);
	}

}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (Subsystem) {
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			FString Adress;
			SessionInterface->GetResolvedConnectString(NAME_GameSession,Adress);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Adress, ETravelType::TRAVEL_Absolute);	
			}
		}
	}
	
	if (Result != EOnJoinSessionCompleteResult::Success) {
		
		JoinButton->SetIsEnabled(true);
	}
}





void UMenu::OnDestroySession(bool bWasSuccessful)
{

}

void UMenu::OnStartSession(bool bWasSuccessful)
{

}

void UMenu::HostButtonClicked()
{

	HostButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);

	}
	
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Join Clicked")));

	}

	if (MultiplayerSessionsSubsystem) 
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}



}
