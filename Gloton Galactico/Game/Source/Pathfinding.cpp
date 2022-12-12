#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "PathFinding.h"

PathFinding::PathFinding() : Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.Create("pathfinding");
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width * height];
	memcpy(map, data, width * height);
}

// Utility: return true if pos is inside the map boundaries
bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y * width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const DynArray<iPoint>* PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const ListItem<PathNode>* PathList::Find(const iPoint& point) const
{
	ListItem<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
ListItem<PathNode>* PathList::GetNodeLowestScore() const
{
	ListItem<PathNode>* ret = NULL;
	int min = 65535;

	ListItem<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.GetF() < min)
		{
			min = item->data.GetF();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::GetF() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
void PathNode::SetGandH(const iPoint& destination)
{
	g = parent->g + COST_TO_MOVE;
	h = pos.DistanceManhattan(destination);
}

void PathNode::FindWalkableAdjacents(PathList& pathList, const iPoint destination)
{
	iPoint cell;
	// north
	cell.Create(pos.x, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	// east
	cell.Create(pos.x + 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	// south
	cell.Create(pos.x, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	// west
	cell.Create(pos.x - 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	// north - west
	cell.Create(pos.x - 1, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));

	// north - east
	cell.Create(pos.x + 1, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));

	// south - west
	cell.Create(pos.x - 1, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));

	// south - east
	cell.Create(pos.x + 1, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
		pathList.list.Add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));

}

void PathNode::FindWalkableAdjacentsLand(PathList& pathList, const iPoint destination)
{
	iPoint cell;
	// south
	cell.Create(pos.x, pos.y + 1);

	if (app->pathfinding->IsWalkable(cell)) {
		pathList.list.Add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));
	}
	else {
		// east
		cell.Create(pos.x + 1, pos.y);
		if (app->pathfinding->IsWalkable(cell))
			pathList.list.Add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));
		// west
		cell.Create(pos.x - 1, pos.y);
		if (app->pathfinding->IsWalkable(cell))
			pathList.list.Add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));
	}
}


// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	//BROFILER_FRAME("CreateThread");
	//BROFILER_THREAD("CreatePath worker");
	//BROFILER_CATEGORY("CreatePath", Profiler::Color::Black);

	if (!IsWalkable(origin) || !IsWalkable(destination) || origin == destination) {
		LOG("Invalid origin or destination: Origin or destination are not walkable or are the same.");
		return -1;
	}

	PathList openList;
	PathList closedList;
	openList.list.Add(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));

	while (openList.list.Count() > 0) {
		ListItem<PathNode>* lowestNode = openList.GetNodeLowestScore();

		PathNode* currNode = &closedList.list.Add(lowestNode->data)->data;
		openList.list.Del(lowestNode);

		if (currNode->pos == destination) {
			last_path.Clear();
			for (PathNode* pathIterator = currNode; pathIterator != nullptr && pathIterator->pos != origin; pathIterator = pathIterator->parent) {
				last_path.PushBack(pathIterator->pos);
			}
			// Adds start node too
			//last_path.PushBack(closedList.list.start->data.pos);
			last_path.Flip();
			LOG("Succesful path: The algorithm has found a path from the origin(%i, %i) to the destination(%i, %i)", origin.x, origin.y, destination.x, destination.y);
			return last_path.Count();
		}

		PathList adjacentNodes;
		currNode->FindWalkableAdjacents(adjacentNodes, destination);

		for (ListItem<PathNode>* adjacentNodeIterator = adjacentNodes.list.start; adjacentNodeIterator != nullptr; adjacentNodeIterator = adjacentNodeIterator->next) {
			if (closedList.Find(adjacentNodeIterator->data.pos) != NULL) {
				continue;
			}
			ListItem<PathNode>* duplicateNode = (ListItem<PathNode>*)openList.Find(adjacentNodeIterator->data.pos);
			if (duplicateNode == NULL) {
				openList.list.Add(adjacentNodeIterator->data);
			}
			else if (adjacentNodeIterator->data.g < duplicateNode->data.g) {
				duplicateNode->data.g = adjacentNodeIterator->data.g;
				duplicateNode->data.parent = currNode;
			}
		}
	}

	LOG("Invalid path: The algorithm has extended to all the possible nodes and hasn't found a path to the destination.");
	return -1;
}

int PathFinding::CreatePathLand(const iPoint& origin, const iPoint& destination)
{
	//BROFILER_CATEGORY("CreatePath", Profiler::Color::Black);
	//LOG("Origin:      x: %i, y: %i", origin.x, origin.y);
	//LOG("Destination: x: %i, y: %i", destination.x, destination.y);
	if (!IsWalkable(origin) || !IsWalkable(destination) || origin == destination) {
		//LOG("Invalid origin or destination: Origin or destination are not walkable or are the same.");
		return -1;
	}

	PathList openList;
	PathList closedList;
	openList.list.Add(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));

	while (openList.list.Count() > 0) {
		ListItem<PathNode>* lowestNode = openList.GetNodeLowestScore();

		PathNode* currNode = &closedList.list.Add(lowestNode->data)->data;
		openList.list.Del(lowestNode);

		if (currNode->pos == destination) {
			last_path.Clear();
			for (PathNode* pathIterator = currNode; pathIterator != nullptr && pathIterator->pos != origin; pathIterator = pathIterator->parent) {
				last_path.PushBack(pathIterator->pos);
			}
			// Adds start node too
			//last_path.PushBack(closedList.list.start->data.pos);
			last_path.Flip();
			LOG("Succesful path: The algorithm has found a path from the origin(%i, %i) to the destination(%i, %i)", origin.x, origin.y, destination.x, destination.y);
			return last_path.Count();
		}

		PathList adjacentNodes;
		currNode->FindWalkableAdjacentsLand(adjacentNodes, destination);

		for (ListItem<PathNode>* adjacentNodeIterator = adjacentNodes.list.start; adjacentNodeIterator != nullptr; adjacentNodeIterator = adjacentNodeIterator->next) {
			if (closedList.Find(adjacentNodeIterator->data.pos) != NULL) {
				continue;
			}
			ListItem<PathNode>* duplicateNode = (ListItem<PathNode>*)openList.Find(adjacentNodeIterator->data.pos);
			if (duplicateNode == NULL) {
				openList.list.Add(adjacentNodeIterator->data);
			}
			else if (adjacentNodeIterator->data.g < duplicateNode->data.g) {
				duplicateNode->data.g = adjacentNodeIterator->data.g;
				duplicateNode->data.parent = currNode;
			}
		}
	}

	//LOG("Invalid path: The algorithm has extended to all the possible nodes and hasn't found a path to the destination.");
	return -1;
}

int PathList::FindCounter(const iPoint& point)
{
	int ret = 0;

	ListItem<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			ret++;
		item = item->next;
	}
	return ret;
}

void PathList::CreateRepeteadsList(PathList& repeatedList, const iPoint nodePoint, int& numRepeteads)
{
	int counter = 0;
	ListItem<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == nodePoint)
		{
			counter++;
			repeatedList.list.Add(item->data);
		}
		item = item->next;
	}
	LOG("repeteads nodes: %i", counter);
	numRepeteads += counter;
}

void PathFinding::CombineTwoRepeteadsList(const PathList& list1, const PathList& list2, PathList& result)
{
	int counter = 0;
	// copy first list
	ListItem<PathNode>* item = list1.list.start;
	while (item)
	{
		result.list.Add(item->data);
		item = item->next;
		counter++;
	}
	// copy second list
	item = list2.list.start;
	while (item)
	{
		result.list.Add(item->data);
		item = item->next;
		counter++;
	}

	LOG("repeteads nodes: %i", counter);
}