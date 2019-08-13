#pragma once
class CNudge
{
public:
	CNudge();
	CNudge(int id, int x, int y, int sc);
	~CNudge();

public:
	int m_id;
	int m_x;
	int m_y;
	int m_sc;

public:
	float scale()
	{
		return (float)m_sc / 100.0f;
	}
};

class CNudgeCollection
{
public:
	CNudgeCollection();
	~CNudgeCollection();
public:
	std::list<CNudge*> m_nudgelist;
public:
	void Add(CNudge* nudge);
	CNudge* Find(int nInstrumentId);
};

