#include <iostream>
#include "PRC.h"

class Car // Sample class, nothing important
{
private:
	std::string Name;
	float Price;
public:
	Car() {};
	Car(std::string _Name, float _Price) {
		Name = _Name;
		Price = _Price;
	}
	std::string GetName() {
		return Name;
	}
	float GetPrice() {
		return Price;
	}
};

int main()
{

	if (!Forest::Init()) {
		return 0;
	}

	PRCManager* PRC = GetPRCManager();
	if (!PRC) return 0;

	Car* Sample = new Car("Nissan GT-R", 18000.35);

	PRC->Add<Car*>(Sample, 1);

	Car* New = PRC->Get<Car*>(1);
	printf("%s\n", New->GetName().c_str());

}
