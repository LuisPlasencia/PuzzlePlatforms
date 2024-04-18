// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyMenuWidget.h"
#include "MyMainMenu.generated.h"

USTRUCT()   
struct FServerData   // lo usaremos para definir los servidores en el setserverlist - a diferencia de las clases, los structs son públicos por defecto y son perfectos para datos que no necesitan lógica
{
	GENERATED_BODY()
	
	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMyMainMenu : public UMyMenuWidget
{
	GENERATED_BODY()

public:
	UMyMainMenu(const FObjectInitializer & ObjectInitializer);  // por que el constructor va así? el constructor de UUserWidget es así y como heredamos de UUserWidget el constructor tiene que seguir ese mismo formato para que pueda ser ejecutado 

	void SetServerList(TArray<FServerData> ServerNames);

	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize();  

private:  
	TSubclassOf<class UUserWidget> ServerRowClass;

	UPROPERTY(meta = (BindWidget))   // se enlaza al botón llamado "Host" automáticamente
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))   
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))   
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))   
	class UButton* CancelJoinMenuButton;

	UPROPERTY(meta = (BindWidget))   
	class UButton* ConfirmJoinMenuButton;

	UPROPERTY(meta = (BindWidget))   
	class UWidgetSwitcher* MenuSwitcher;   // para comprobar si esta chucha se llama así podemos ir al fichero cpp y mirar si hay algún componente con ese nombre en include (buildeamos y vemos si se ha incluido o con el autocomplete)

	UPROPERTY(meta = (BindWidget))   
	class UWidget* JoinMenu;   // por que uwidget y no size box? porque puede cambiar más adelante así que no nos viene bien ser tan específicos. Realmente solo me importa que esté en la raíz de la jerarquía para identificarlo y como todos los botones y componentes tienen como padre UWidget pues me sirve (also: If we don't know the type of a widget that we want to point do just point to UWidget)

	UPROPERTY(meta = (BindWidget))   
	class UWidget* MainMenu; 

	UPROPERTY(meta = (BindWidget))   
	class UWidget* HostMenu; 

	UPROPERTY(meta = (BindWidget))   
	class UEditableTextBox* ServerHostName; 

	UPROPERTY(meta = (BindWidget))   
	class UButton* CancelHostMenuButton; 

	UPROPERTY(meta = (BindWidget))   
	class UButton* ConfirmHostMenuButton; 

	UPROPERTY(meta = (BindWidget))   
 	class UPanelWidget* ServerList;  // realmente lo que tenemos es un ScrollBox, per este hereda del UPanelWidget, el cual es el elemento encargado de poder tener hijos y este es el funcionamiento que queremos realmente. No vamos a utilizar cosas específicas del scrollbox sino del upanelwidget que es el padre y que es el elemento capaz de contener hijos.

	// UPROPERTY(meta = (BindWidget))   
	// class UEditableTextBox* IPAddressField; 

	UFUNCTION()  // Dynamic callback events need to be ufunction
	void HostServer();   // en este caso el callback del onclicked no pide arguments (mirar documentación)

	UFUNCTION()  
	void JoinServer();

	UFUNCTION()  
	void OpenHostMenu();

	UFUNCTION()  
	void OpenJoinMenu();

	UFUNCTION()  
	void OpenMainMenu();

	UFUNCTION()  
	void QuitPressed();

	TOptional<uint32> SelectedIndex;   // Usamos TOptional en vez de un puntero porque el puntero ocupa memoria en el heap y encima luego hay que hacerle dealocate y es caro.  
	// TOptional es alocated en el lugar del objeto al que apunta. Puede ser null o tener un valor asignado. Tendremos que comprobarlo con llamadas si tiene o no tiene un valor en ese momento y es muy barato. Perfecto para señalar el índice de la lista de servidores ya que esta puede ser null pero tampoco queremos un puntero.
	// alternativa es no usar toptional y simplemente usar un número negativo cuando no tenemos elementos, pero un TOptional es mejor para la memoria porque es nulleable (unset).

	void UpdateChildren();
};
