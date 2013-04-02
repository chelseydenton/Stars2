#include "CImg.h"
#include <queue>

using namespace cimg_library;
using namespace std;

class node
{
public:
	int x, y, val;
	node(int x, int y, int val)
	{
		this->x = x;
		this->y = y;
		this->val = val;
	}
	node()
	{
		x = 0;
		y = 0;
		val = 0;
	}
};

class edge
{
public:
	int weight;
	node node1, node2;
	edge(node node1, node node2)
	{
		this->node1 = node1;
		this->node2 = node2;
		weight = abs(node1.val - node2.val);
	}
	edge()
	{
		weight = 0;
	}
};

class mycomparison
{
  bool reverse;
public:
  mycomparison(const bool& revparam=false)
    {reverse=revparam;}
  bool operator() (const edge& lhs, const edge&rhs) const
  {
	  if (reverse) return (lhs.weight>rhs.weight);
	  else return (lhs.weight<rhs.weight);
  }
};

int main(int argc,char *argv[])
{
	CImg<unsigned char> image("C:\\Users\\cdent_000\\Pictures\\STARS.bmp");
	printf("%i\n", image.depth());
	//printf("%i, %i\n", image.height(), image.width());

	CImg<unsigned char> blackAndWhite(image.width(), image.height(), 1, 1);
	
	unsigned char *r = image.data(image.width(), image.height(), 0, 0);
	unsigned char *g = image.data(image.width(), image.height(), 0, 1);
	unsigned char *b = image.data(image.width(), image.height(), 0, 2);
		
	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			int ri = image(j, i, 0, 0);
			int gi = image(j, i, 0, 1);
			int bi = image(j, i, 0, 2);
			blackAndWhite(j, i, 0, 0)  = 0.2989f*ri +  0.5870f*gi + 0.1140f*bi;
		}
	}

	priority_queue<edge,vector<edge>,mycomparison> edges  = priority_queue<edge,vector<edge>,mycomparison>(mycomparison());
	vector<vector<node>> nodes (image.height(), vector<node>(image.width(), node()));

	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			nodes[i][j] = node(j, i, blackAndWhite(j, i, 0, 0));
		}
	}

	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			bool endCol = j == (image.width()-1);
			bool endRow = i == (image.height() -1);
			if(endCol && endRow)
			{

			}
			else if(endCol)
			{
				edges.push(edge(nodes[i][j], nodes[i+1][j]));
			}
			else if(endRow)
			{
				edges.push(edge(nodes[i][j], nodes[i][j+1]));
			}
			else
			{
				edges.push(edge(nodes[i][j], nodes[i][j+1]));
				edges.push(edge(nodes[i][j], nodes[i+1][j]));
				edges.push(edge(nodes[i][j], nodes[i+1][j+1]));
			}
		}
	}
	
	CImgDisplay main_disp(image,"Stars"), disp_2(blackAndWhite, "greyscale");
	while (!main_disp.is_closed() && !disp_2.is_closed()) {
		main_disp.wait();
		disp_2.wait();
	}
	return 0;
}