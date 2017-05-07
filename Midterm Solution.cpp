#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

class Point;
class Building;
class Distribution;
class Logistics;
class Store;

class Point
{
	public:
		const int x;
		const int y;
		Point(int x,int y);
		int manhattonDistance(const Point& to) const;
};

class Building
{
    protected:
        Building(int id, Point& position,int cost);
        virtual ~Building();
    public:

    	int revenue;
 		int expense;
 		map<int,Distribution*> distribution;
 		static int costPerKM;
        const int id;
        const Point& position;
        const int cost;
        //getter
        int getRevenue() const;
        int getExpense() const;
        int manhattonDistance(const Building& to);
        int getNet()const;
        static int compareNet(const Building& b1,const Building& b2);
        double getOER()const;//OperatingExpenseRatio
        static int compareOER(const Building& b1,const Building& b2);//OperatingExpenseRatio
        static int send(Logistics& from, Store& to, int units);

};

/** Zhen start */
class Logistics: public Building //subclass
{
	private:
		const int capacity;
		map<int,Store*> possibleStores;
	public:
		int unsold;
		// Constructors
		Logistics(int id, Point& position, int cost, int capacity);
		// Functions
		int send(Store& to, int units);
		void include(Store& s);
		void include(Store* ss, int sNum);
		void include(Store** ss, int sNum);
		// Accessors
		int getCapacity() const;
		map<int,Store*>& getPossibleStores();
		int getUnsold() const;
};

class Store: public Building//subclass
{

	private:
		const int demand;
		map<int,Logistics*> possibleLogistics;
		const int price;
	public:
		int unsatisfied;
		// Constructors
		Store(int id, Point& position, int cost, int demand, int price);
		// Functions
		int receive(Logistics& from, int units);
		void include(Logistics& l);
		void include(Logistics* ls, int lNum);
		void include(Logistics** ls, int lNum);
		// Accessors
		int getDemand() const;
		map<int,Logistics*>& getPossibleLogistics();
		int getPrice() const;
		int getUnsatisfied() const;
};

class Distribution
{
	private:
		Logistics& from;
		Store& to;
	public:
		// Variables
		const int price;
		const int unitCost;
		int units;
		// Constructors
		Distribution(Logistics& from, Store& to);
		// Functions
		int getUnitNet();
		int getNet();
		// Accessors
		Logistics& getFrom();
		Store& getTo();
};
/** Zhen end */


/** JasonBaby start */
class Plan
{
private:
    map<int,Logistics*> logistics;
    map<int,Store*> stores;
    int revenue;
    int expense;
    map<int,Logistics*> unsold;
    map<int,Store*> unsatisfied;
public:
	// Static fields
	static int numLogistics;
	static int numStores;
	// Constructors
	Plan(const Plan& p);
	Plan& operator=(const Plan& p);
	Plan(Logistics**& ls, int lNum, Store**& ss, int sNum);
	// Functions
	int getNet() const;
	string toString() const;
	bool remove(Building* building);
	void update();
	// Accessors
    map<int,Logistics*>& getLogistics();
    map<int,Store*>& getStores();
    int getRevenue() const;
    int getExpense() const;
    map<int,Logistics*>& getUnsold();
    map<int,Store*>& getUnsatisfied();
};
/** JasonBaby end */

int main()
{

	return 0;
}

/** JasonBaby start */

// Plan
// Constructors
Plan::Plan(Logistics**& ls, int lNum, Store**& ss, int sNum)
{
    revenue = 0;
    expense = 0;

    for (int i = 0; i < lNum; i++)
	{
		Logistics* l = ls[i];
        logistics[l->id] = l;
        unsold[l->id] = l;
        expense += l->expense;
	}

    for (int i = 0; i < sNum; i++)
	{
		Store* s = ss[i];
        stores[s->id] = s;
        unsatisfied[s->id] = s;
        expense += s->expense;
	}

    for (int i = 0; i < lNum; i++)
	{
		ls[i]->include(ss, sNum);
	}
	for (int i = 0; i < sNum; i++)
	{
		ss[i]->include(ls, lNum);
	}
}

Plan::Plan(const Plan& p)
{
	*this = p;
}

Plan& Plan::operator=(const Plan& p)
{
    revenue = p.revenue;
    expense = p.expense;

    for (map<int,Logistics*>::const_iterator it = p.logistics.begin();
		it != p.logistics.end(); it++)
	{
		Logistics l = *it->second;
		logistics[it->first] = &l;
	}
    for (map<int,Store*>::const_iterator it = p.stores.begin();
		it != p.stores.end(); it++)
	{
		Store s = *it->second;
		stores[it->first] = &s;
	}
    for (map<int,Logistics*>::const_iterator it = p.unsold.begin();
		it != p.unsold.end(); it++)
	{
		Logistics l = *it->second;
		unsold[it->first] = &l;
	}
    for (map<int,Store*>::const_iterator it = p.unsatisfied.begin();
		it != p.unsatisfied.end(); it++)
	{
		Store s = *it->second;
		unsatisfied[it->first] = &s;
	}

    return *this;
}

int Plan::getNet() const
{
	return revenue - expense;
}

string Plan::toString() const
{
	stringstream result;

	result << logistics.size();
	for(auto it = logistics.begin();
		it != logistics.end(); it++)
	{
		int id = it->first;
		result << " " << id;
	}

	result << endl;
	result << stores.size();
	for(auto it = stores.begin();
		it != stores.end(); it++)
	{
		int id = it -> first;
		result << " " << id;
	}

	for(int i = 1; i <= numStores; i++)
	{
		result << endl;
		if(stores.find(i) == stores.end())
		{
			if (1 <= numLogistics)
			{
				result << 0;
			}
			for (int j = 2; j <= numLogistics; j++)
			{
				result << " 0";
			}
			result << endl;
			continue;
		}

		Store* s = stores.find(i)->second;
		map<int,Distribution*>& ds = s->distribution;
		for(int j = 1; j <= numLogistics; j++)
		{
			result << " ";
			auto dit = ds.find(j);
            if (dit == ds.end())
			{
				result << 0;
			}
			else
			{
				Distribution* d = dit->second;
				result << d->units;
			}
		}
	}

	return result.str();
}

bool Plan::remove(Building* building)
{
	if (Store* s = dynamic_cast<Store*> (building))
	{
        int id = s->id;
		stores.erase(id);
		for (map<int,Distribution*>::iterator it = s->distribution.begin();
			it != s->distribution.end(); it++)
		{
			Distribution*& d = it->second;
			Logistics& l = d->getFrom();
			unsold[l.id] = &l;

			int revenue = d->price * d->units;
			int expense = d->unitCost * d->units;
			l.revenue -= revenue;
			l.expense -= expense;
			l.unsold += d->units;

            l.getPossibleStores().erase(id);
            l.distribution.erase(id);
            delete d;
		}
		revenue -= s->revenue;
		expense -= s->expense;
		delete s;
	}
	else
	{
		Logistics* l = dynamic_cast<Logistics*> (building);
        int id = l->id;
		logistics.erase(id);
		for (map<int,Distribution*>::iterator it = l->distribution.begin();
			it != l->distribution.end(); it++)
		{
			Distribution*& d = it->second;
			Store& s = d->getTo();
			unsatisfied[s.id] = &s;

			int revenue = d->price * d->units;
			int expense = d->unitCost * d->units;
			s.revenue -= revenue;
			s.expense -= expense;
			s.unsatisfied += d->units;

            s.getPossibleLogistics().erase(id);
            s.distribution.erase(id);
            delete d;
		}
		revenue -= l->revenue;
		expense -= l->expense;
		delete l;
	}
}

void Plan::update()
{
	for (map<int,Logistics*>::iterator lit = unsold.begin();
		lit != unsold.end(); lit++)
	{
        for (map<int,Store*>::iterator sit = unsatisfied.begin();
			sit != unsatisfied.end(); sit++)
		{
            lit->second->include(*sit->second);
            sit->second->include(*lit->second);
		}
	}

	bool done = false;

	while (!done)
	{
		Distribution* bestD = nullptr;
		for (map<int,Logistics*>::iterator lit = unsold.begin();
			lit != unsold.end(); lit++)
		{
			Logistics* l = lit->second;
			map<int,Store*>& possible = l->getPossibleStores();
			for (map<int,Store*>::iterator sit = possible.begin();
				sit != possible.end(); sit++)
			{
				Store* s = sit->second;
				Distribution* d = new Distribution(*l, *s);
				d->units++;

				if (bestD == nullptr || d->getNet() > bestD->getNet())
				{
					bestD = d;
				}
			}
		}

		if (bestD == nullptr)
		{
            done = true;
		}
		else
		{
			Logistics& l = bestD->getFrom();
			Store& s = bestD->getTo();
			int units = min(l.getUnsold(), s.getUnsatisfied());
			Building::send(l, s, units);

			int price = bestD->price;
			int unitCost = bestD->unitCost;
			revenue += price * units;
			expense += unitCost *= units;

			if (l.getUnsold() == 0)
			{
                unsold.erase(l.id);
			}
			if (s.getUnsatisfied() == 0)
			{
				unsatisfied.erase(s.id);
			}
		}
	}
}

// Accessors
map<int,Logistics*>& Plan::getLogistics()
{
	return logistics;
}

map<int,Store*>& Plan::getStores()
{
	return stores;
}

int Plan::getRevenue() const
{
	return revenue;
}

int Plan::getExpense() const
{
	return expense;
}

map<int,Logistics*>& Plan::getUnsold()
{
	return unsold;
}

map<int,Store*>& Plan::getUnsatisfied()
{
	return unsatisfied;
}
/** JasonBaby end */

/** HanjuTsai start*/
Point:: Point(int x,int y):x(x),y(y)
{
}
int Point:: manhattonDistance(const Point& to)const
{
    int manhattonDistance = 0;
    manhattonDistance = abs(x - to.x) + abs(y - to.y);
    return manhattonDistance;
}
Building::Building(int id,Point& position,int cost): id(id), position(position), cost(cost)
{
}
int Building::getRevenue()const
{
    return revenue;
}
int Building::getExpense()const
{
    return expense;
}
int Building::getNet()const
{
    return revenue - expense;
}
int Building::manhattonDistance(const Building& to)//because the point reference is a const
{
    int manhattonDistance = 0;
    manhattonDistance = (this->position.x - to.position.x)+(this->position.y - to.position.y);//not quite sure
    return manhattonDistance;
}
int Building ::compareNet(const Building& b1,const Building& b2)
{
    if(b1.getNet() < b2.getNet())
    {
        return -1;
    }
    else if(b1.getNet() == b2.getNet())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
double Building::getOER()const
{
    double OER = 0;
    OER = static_cast<float>(revenue)/expense;
    return OER;
}
int Building:: compareOER(const Building& b1,const Building& b2)
{
    if(b1.getOER() < b2.getOER())
    {
        return -1;
    }
    else if(b1.getOER() == b2.getOER())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
int Building:: send(Logistics& from, Store& to, int units)
{
    for(int i = 0; i < from.distribution.size(); i++)
    {
        if((from.id == (from.distribution[i]->getFrom()).id)&&(to.id == (from.distribution[i]->getTo()).id))
        {
            units += from.distribution[i]->units;
        }
    }
    from.send(to,units);
    to.receive(from,units);
    return units;
}
Building:: ~Building()
{
    distribution.clear();
}
/** HanjuTsai end */

/** Zhen start */
// Logistics
// Logistics-Constructors
Logistics::Logistics(int id, Point& position, int cost, int capacity)
	: Building(id, position, cost), capacity(capacity)
{
	unsold = capacity;
}
// Logistics-Functions
int Logistics::send(Store& to, int units)
{
	this->unsold -= units;
	Distribution dis(*this, to);
	revenue += to.getPrice() * units;
	expense += dis.unitCost * units;
	if(this->unsold == 0)
	{
		to.getPossibleLogistics().erase(this->id);
	}
	return units;
}
void Logistics::include(Store& s)
{
	if (possibleStores.find(s.id) != possibleStores.end())
	{
		return;
	}
	if(s.getDemand() == 0)
	{
		return;
	}
	if(Building::costPerKM * this->manhattonDistance(s) < s.getPrice())
	{
		possibleStores[s.id] = &s;
	}
}
void Logistics::include(Store* ss, int sNum)
{
	for(int i=0; i<sNum; i++)
	  this->include(ss[i]);
}
void Logistics::include(Store** ss, int sNum)
{
	for(int i=0; i<sNum; i++)
	  this->include(*ss[i]);
}
// Logistics-Getters
int Logistics::getCapacity() const
{
	return capacity;
}
map<int, Store*>& Logistics::getPossibleStores()
{
	return possibleStores;
}
int Logistics::getUnsold() const
{
	return unsold;
}

// Store
// Store-Constructors
Store::	Store(int id, Point& position, int cost, int demand, int price)
	: Building(id, position, cost), demand(demand), price(price)
{
	unsatisfied = demand;
}
// Store-Functions
int Store::receive(Logistics& from, int units)
{
	this->unsatisfied -= units;
	Distribution dis(from, *this);
	revenue += this->price * units;
	expense += dis.unitCost * units;
	if(this->unsatisfied==0)
	  from.getPossibleStores().erase(this->id);
	return units;
}
void Store::include(Logistics& l)
{
	if(possibleLogistics.find(l.id) != possibleLogistics.end())
	{
		return;
	}
	if(l.getCapacity() == 0)
	{
		return;
	}
	if(Building::costPerKM * this->manhattonDistance(l) < price)
	{
		possibleLogistics[l.id] = &l;
	}
}
void Store::include(Logistics* ls, int lNum)
{
	for(int i=0; i<lNum; i++)
	{
		this->include(ls[i]);
	}
}
void Store::include(Logistics** ls, int lNum)
{
	for(int i=0; i<lNum; i++)
	{
		this->include(*ls[i]);
	}
}
// Store-Getters
int Store::getDemand() const
{
	return demand;
}
map<int, Logistics*>& Store::getPossibleLogistics()
{
	return possibleLogistics;
}
int Store::getPrice() const
{
	return price;
}
int Store::getUnsatisfied() const
{
	return unsatisfied;
}


// Distribution
// Distribution-Constructors
Distribution::Distribution(Logistics& from, Store& to)
	: from(from)
	, to(to)
	, price(to.getPrice())
	, unitCost(Building::costPerKM * from.manhattonDistance(to))
	, units(0)
{}


// Distribution-Functios
int Distribution::getUnitNet()
{
	int getUnitNet = price-unitCost;
	return getUnitNet;
}
int Distribution::getNet()
{
	int getNet = units * this->getUnitNet();
	return getNet;
}
// Distribution-Getters
Logistics& Distribution::getFrom()
{
	return from;
}
Store& Distribution::getTo()
{
	return to;
}


/** Zhen end */
