// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/09_Goblin/Goblin.h"


AGoblin::AGoblin()
{
	
	HelmetComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetComponent"));
	HelmetComponent->SetupAttachment(GetMesh());
	
	SkirtComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkirtComponent"));
	SkirtComponent->SetupAttachment(GetMesh());
	
	LegComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegComponent"));
	LegComponent->SetupAttachment(GetMesh());
	
	BracerComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BracerComponent"));
	BracerComponent->SetupAttachment(GetMesh());
	
	BodyComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyComponent"));
	BodyComponent->SetupAttachment(GetMesh());
	
	WeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetupAttachment(GetMesh());


	HelmetComponent->SetLeaderPoseComponent(GetMesh());
	SkirtComponent->SetLeaderPoseComponent(GetMesh());
	LegComponent->SetLeaderPoseComponent(GetMesh());
	BracerComponent->SetLeaderPoseComponent(GetMesh());
	BodyComponent->SetLeaderPoseComponent(GetMesh());
	
	
}

AGoblinRogue::AGoblinRogue()
{
	if(WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(),"Sword");
	}

}

AGoblinWarrior::AGoblinWarrior()
{
	if(WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(),"Sword");
	}
	
	ShieldComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldComponent"));
	ShieldComponent->SetupAttachment(GetMesh(),"Shield");
}

AGoblinMage::AGoblinMage()
{
	if(WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(),"Staff");
	}
}

AGoblinCrystallized::AGoblinCrystallized()
{
	CrystallizedArmComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedArmComponent"));
	CrystallizedArmComponent->SetupAttachment(GetMesh());
	
	CrystallizedBodyComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedBodyComponent"));
	CrystallizedBodyComponent->SetupAttachment(GetMesh());
	
	CrystallizedLegBottomComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedLegBottomComponent"));
	CrystallizedLegBottomComponent->SetupAttachment(GetMesh());
	
	CrystallizedLegTopComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedLegTopComponent"));
	CrystallizedLegTopComponent->SetupAttachment(GetMesh());
	
	CrystallizedHeadComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedHeadComponent"));
	CrystallizedHeadComponent->SetupAttachment(GetMesh());
	
	CrystallizedShoulderComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedShoulderComponent"));
	CrystallizedShoulderComponent->SetupAttachment(GetMesh());

	CrystallizedArmComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedBodyComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedLegBottomComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedLegTopComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedHeadComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedShoulderComponent->SetLeaderPoseComponent(GetMesh());
}

AGoblinBattleMage::AGoblinBattleMage()
{
	if(WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(),"Sword");
	}
	
	StaffComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaffComponent"));
	StaffComponent->SetupAttachment(GetMesh(),"StaffMirror");
}
