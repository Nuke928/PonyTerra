#ifndef ITEM_PICKAXE_H
#define ITEM_PICKAXE_H

#include "Item_MiningTool.h"

class CTile;
class ITexture;

class CItem_Pickaxe : public CItem_MiningTool
{
public:
	CItem_Pickaxe(CEntity *owner);
	~CItem_Pickaxe();

	void Initialize();
	void Update(float dtTime);

	bool OnUse();

	void Draw(CVector2f pos);

	std::string GetName() const { return "Pickaxe"; }

private:
	ITexture *tilesheet;
};

#endif