#pragma once


#include <XYZ.h>


class Map
{
public:
	Map();

	void Init(int width,std::shared_ptr<XYZ::Material> material);
	void Destroy();

	void SetTile(int row, int col);
	void Generate(int numOfTiles);

private:
	XYZ::Entity m_BackGround;
	std::vector<XYZ::Entity> m_Walls;
	std::shared_ptr<XYZ::Material> m_Material;
	int m_Width;
	bool m_Init;
};