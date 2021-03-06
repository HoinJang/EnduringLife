#include "stdafx.h"
#include "cObject.h"
#include "cMyNamespace.h"
#include "cThreadFunc.h"

cObject::cObject() : is_use(false), id(-1), x((float)(BOARD_WIDTH/2)), z((float)(BOARD_WIDTH/2)), sight_x(0.0f), sight_z(1.0f)
{
	exover.command = E_RECV;
	scene = TOWN;
	animation_num = 0;

	p_type = MALE;
}

cObject::~cObject()
{
}

void cObject::UpdatesSectorSearch()
{
	RECT rc;
	rc.left = x - VIEW_RADIUS;
	rc.right = x + VIEW_RADIUS;
	rc.top = z - VIEW_RADIUS;
	rc.bottom = z + VIEW_RADIUS;

	AdjustRect(rc);

	near_sector.clear();
	
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.left, rc.top));
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.right, rc.top));
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.left, rc.bottom));
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.right, rc.bottom));

	near_list.clear();

	for (auto sector : near_sector)
	{
		sector->sector_mutex.lock();
		unordered_set<UINT> temp = sector->GetInPlayer();
		sector->sector_mutex.unlock();
		for (auto near_player : temp)
		{
			if (near_player == id)
				continue;
			if (CanSee(near_player, id) == true && objects[near_player]->GetIsUse())
			{
				near_list.insert(near_player);
			}
		}
	}
	//Sector 검색후 Sector내에 near_pleyer를 sector_search(list)에 삽입
	//near_list 업데이트

}
void cObject::AdjustRect(RECT& rc)
{
	if (rc.left < 0)
		rc.left = 0;
	if (rc.top < 0)
		rc.top = 0;
	if (rc.right >= BOARD_HEIGHT - 1)
		rc.right = BOARD_HEIGHT - 1;
	if (rc.bottom >= BOARD_HEIGHT - 1)
		rc.bottom = BOARD_HEIGHT - 1;
}
bool cObject::CanSee(const UINT i, const UINT j)
{
	int x = objects[i]->GetX() - objects[j]->GetX();
	int y = objects[i]->GetZ() - objects[j]->GetZ();

	int dist = x * x + y * y;

	return (dist <= (VIEW_RADIUS) * (VIEW_RADIUS));
}
void cObject::InsertSector(vector<cSector*>& near_sector, cSector* sector)
{
	auto iter = find(near_sector.begin(), near_sector.end(), sector);

	if (iter == near_sector.end())
		near_sector.push_back(sector);
}

void cObject::NearlistClear()
{
	near_list.clear();
}

