#include "CImg.h"
#include <queue>
#include <vector>

using namespace cimg_library;
using namespace std;

class DisjointSet
{
    public:
        class   Node 
        {
            public:
                int x, y;
                int rank;
                Node* parent;
                Node() : x(0), y(0), rank(0), parent(this) { }
        };
        Node* find(Node*);
        bool _union(Node*, Node*); // Union
};
DisjointSet::Node*   DisjointSet::find(DisjointSet::Node* n)
{
    if (n != n->parent) {
        n->parent = find(n->parent); //recursively find the parent
    }
    return n->parent;
}
bool DisjointSet::_union(DisjointSet::Node* x, DisjointSet::Node* y)
{
	x = find(x);
    y = find(y);

	if(x == y) //discard edges which would create a cycle
		return false;

    if (x->rank > y->rank) {
        y->parent = x;
    } else {
        x->parent = y;
        if (x->rank == y->rank) {
            ++(y->rank);
        }
    }
	return true; //return true if the edge is included
}

class edge
{
public:
	DisjointSet::Node *node1;
	DisjointSet::Node *node2;
	int val; //edge weight
	edge()
	{

	};
	edge(DisjointSet::Node *node1, DisjointSet::Node *node2, int val)
	{
		this->node1 = node1;
		this->node2 = node2;
		this->val = val;
	}
};
bool compare(edge i, edge j)
{
	return j.val > i.val;
}
bool compare2(edge i, edge j)
{
	return j.val < i.val;
}

const unsigned char color[] = { 0,255,0 };
const unsigned char color2[] = { 255,0,0 };
DisjointSet set;
int cuttOff = 225;
DisjointSet::Node** nodes = NULL;
bool** stars = NULL;
vector<edge> edges;
vector<DisjointSet::Node> starNodes;


char* fileName = "C:\\Users\\cdent_000\\Documents\\GitHub\\Stars2\\STARSOrig.bmp";
CImg<unsigned char> image(fileName);
CImg<unsigned char> blackAndWhite(image.width(), image.height(), 1, 1);

void Reset()
{
	starNodes = vector<DisjointSet::Node>();
	edges = vector<edge>();
	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			stars[i][j] = false;
			nodes[i][j].parent = &nodes[i][j];
			nodes[i][j].rank = 0;
			nodes[i][j].x = j;
			nodes[i][j].y = i;
		}
	}
}
void GetEdges()
{
	//find the edges between every adjacent pixel by each pixel accounting for just half the adjacent pixels to avoid redundant edges
	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			if(j != image.width() -1)
				edges.push_back(edge(&nodes[i][j], &nodes[i][j+1],  .5*(blackAndWhite(j, i, 0, 0) + blackAndWhite(j+1, i, 0, 0))));
			if(i != image.height() -1)
				edges.push_back(edge(&nodes[i][j], &nodes[i+1][j],  .5*(blackAndWhite(j, i, 0, 0) + blackAndWhite(j, i+1, 0, 0))));
			if(i != image.height() -1 && j != image.width() - 1)
				edges.push_back(edge(&nodes[i][j], &nodes[i+1][j+1],  .5*(blackAndWhite(j, i, 0, 0) + blackAndWhite(j+1, i+1, 0, 0))));
			if(i != 0 && j != image.width() -1)						  
				edges.push_back(edge(&nodes[i][j], &nodes[i-1][j+1],  .5*(blackAndWhite(j, i, 0, 0) + blackAndWhite(j+1, i-1, 0, 0))));
		}
	}
}
void CreateForest()
{
	//assuming the edges have already been sorted, begin adding them to the tree
	//merge discards edge if it would create a cycle
	while(!edges.empty() && edges.back().val >= cuttOff)
	{
		set._union(edges.back().node1, edges.back().node2);
		edges.pop_back();
	}
}
void FindStars()
{
	//any node who has a node (other than itself) which uses it as it's root node can be considered the
	// root of a tree, so set a boolean to true at each pixels where there is a root
	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			DisjointSet::Node * node = &nodes[i][j];
			DisjointSet::Node * parent = set.find(node);
			if(node != parent)
			{
				stars[parent->y][parent->x] = true;
			}
		}
	}
	//use the booleans to create an array of star nodes
	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			if(stars[i][j])
				starNodes.push_back(nodes[i][j]);
		}
	}
}
CImg<unsigned char> ConstellationImage()
{
	vector<edge> edges;
	vector<edge> usedEdges;
	//find all the edges and reset the connections between the star nodes
	for(int node1 = 0; node1 < starNodes.size(); node1++)
	{
		starNodes[node1].rank = 0;
		starNodes[node1].parent = &starNodes[node1];
		for(int node2 = node1 + 1; node2 < starNodes.size(); node2++)
		{
			edges.push_back(edge(&starNodes[node1], &starNodes[node2], sqrt(pow(starNodes[node1].x - starNodes[node2].x, 2) + pow(starNodes[node1].y - starNodes[node2].y, 2))));
		}
	}
	//create a minimum spanning tree of the stars 
	std::sort(edges.begin(), edges.end(), compare2);
	while(!edges.empty() && usedEdges.size() < starNodes.size())//the minimum spanning tree will only contain n-1 edges
	{
		if (set._union(edges.back().node1, edges.back().node2))
			usedEdges.push_back(edges.back()); //save the edges to be displayed later
		edges.pop_back();
	}
	CImg<unsigned char> image(fileName);
	int i = 0;
	int remove = usedEdges.size()/10;
	while(!usedEdges.empty())
	{
		//draw the edges on the minimum spanning tree, but skip the last few to show it like a forest so it's not one big constellation
		if(i > remove)
			image.draw_line(usedEdges.back().node1->x, usedEdges.back().node1->y, usedEdges.back().node2->x, usedEdges.back().node2->y, color);
		usedEdges.pop_back();
		i++;
	}
	//draw dots at all the stars
	for (std::vector<DisjointSet::Node>::iterator it = starNodes.begin() ; it != starNodes.end(); ++it)
	{
			image.draw_circle(it->x, it->y, 2, color2);
	}
	return image;
}

int main(int argc,char *argv[])
{
			
	stars = new bool*[image.height()];
	nodes = new DisjointSet::Node*[image.height()];
	for(int i=0; i<image.height(); i++)
	{
		stars[i] = new bool[image.width()];
		nodes[i] = new DisjointSet::Node[image.width()];
	}
	for(int i=0; i<image.height(); i++)
	{
		for(int j=0; j<image.width(); j++)
		{
			int ri = image(j, i, 0, 0);
			int gi = image(j, i, 0, 1);
			int bi = image(j, i, 0, 2);
			int val = (char)(0.2989f*ri +  0.5870f*gi + 0.1140f*bi);
			blackAndWhite(j, i, 0, 0)  = val;
		}
	}
	Reset();
	GetEdges();
	std::sort(edges.begin(), edges.end(), compare);
	CreateForest();
	FindStars();
	CImgDisplay disp(image, "Original Image");
	CImgDisplay const_disp(ConstellationImage(), "Constelations");
	while (true){
		//calc up to need threshold
		Reset();
		GetEdges();
		std::sort(edges.begin(), edges.end(), compare);

		printf("Current Edge Weight Cut Off: %i\n", cuttOff);
		printf("Enter new cut off to recalculate stars now (range 0 - 255)\n");
		int tmp = 0;
		scanf("%i", &tmp);
		while(tmp > 255 || tmp < 0)
		{
			printf("Invalid cut off, enter an integer between 0 and 255\n");
			scanf("%i", &tmp);
		}
		printf("Recalculating...\n");
		cuttOff = tmp;
		//continue calc
		CreateForest();
		FindStars();
		const_disp.close();
		const_disp = CImgDisplay(ConstellationImage(), "Constelations");
		printf("done\n");
	}
	return 0;
}
