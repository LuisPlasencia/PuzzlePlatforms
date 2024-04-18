// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"


AMovingPlatform::AMovingPlatform() 
{
    PrimaryActorTick.bCanEverTick = true;

    SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SetReplicates(true);   // haremos de este actor algo que sea replicable (desde el servidor hacia el cliente), es decir que si se actualiza en servidor se actualiza en cliente
        SetReplicateMovement(true);
    }
    
    GlobalStartLocation = GetActorLocation();
    // Transformation of position vectors is applied in the order: Scale -> Rotate -> Translate. Transformation of direction vectors is applied in the order: Scale -> Rotate. 
    GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);   // pasamos de local a global  (transform - local a global;  inverse transform - global a local)

}

void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ActiveTriggers > 0)
    {
        if (HasAuthority())   // retorna true si somos el servidor, false si somos solo cliente (si el actor es replicable, se actualizará también en el cliente porque desde el servidor le pasaremos la información a los clientes)
            {
                FVector Location = GetActorLocation();

                float JourneyLength = (GlobalTargetLocation - GlobalStartLocation).Size();
                float JourneyTravelled = (Location - GlobalStartLocation).Size();
                if(JourneyTravelled >= JourneyLength)
                {
                    FVector swap = GlobalStartLocation;
                    GlobalStartLocation = GlobalTargetLocation;
                    GlobalTargetLocation = swap;
                }
                //  FVector::Distance(Location, GlobalTargetLocation)    // mejor forma, me ahorro variables 

                FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal(); 
                Location += Speed * DeltaTime * Direction;
                SetActorLocation(Location);
            }    

            // if (!HasAuthority())   // Not on server = client . Cambios en el cubo solo en cliente no se comunican al servidor automáticamente y por tanto solo cambiarán en el cliente actual. Aún así, el servidor sigue teniendo la verdad así que el cliente no se podrá mover al sitio en el cubo está en el servidor aunque sea invisible, a diferencia que entre cliente a servidor contra un actor no replicable
            // {
            //     FVector Location = GetActorLocation();
            //     Location += FVector(Speed * DeltaTime, 0, 0);
            //     SetActorLocation(Location);
            // }   
    }

    

}

// Definition : This is how it is made
// Declaration : I'm gonna make it
void AMovingPlatform::AddActiveTrigger()
{
    ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
    if(ActiveTriggers > 0)
    {
        ActiveTriggers--;
    }
}

