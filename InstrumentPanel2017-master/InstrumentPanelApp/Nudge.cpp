#include "stdafx.h"
#include "Nudge.h"

extern bool bDefaultLayout;


CNudge::CNudge()
{
	m_id = -1;
	m_sc = 100;
	m_x = 0;
	m_y = 0;
}

CNudge::CNudge(int id, int x, int y, int sc)
{
	m_id = id;
	m_x = x;
	m_y = y;
	m_sc = sc;
}

CNudge::~CNudge()
{
}

/*--------------------------------------*/

CNudgeCollection::CNudgeCollection()
{}

CNudgeCollection::~CNudgeCollection()
{
	for (auto nudge : m_nudgelist)
	{
		delete nudge;
	}
	m_nudgelist.clear();
}

void CNudgeCollection::Add(CNudge* nudge)
{
	m_nudgelist.push_back(nudge);
}

CNudge* CNudgeCollection::Find(int nInstrumentId)
{
	for (auto nudge : m_nudgelist)
	{
		if (nudge->m_id == nInstrumentId)
			return (bDefaultLayout==1)?NULL:nudge;
	}
	return NULL;
}
