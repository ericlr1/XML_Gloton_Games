#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"
#include "Point.h"
#include "DynArray.h"
#include "EnemyGround.h"
#include "List.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255
#define COST_TO_MOVE 1

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, PathNode* parent);
	PathNode(const PathNode& node);

	// Calculates this tile score
	int GetF() const;
	// Calculate the F for a specific destination tile
	//int GetLandF() const;
	void SetGandH(const iPoint& destination);
	// Find walkable adjacents
	void FindWalkableAdjacents(PathList& list, const iPoint destination);
	void FindWalkableAdjacentsLand(PathList& pathList, const iPoint destination);
	void FindWalkableAdjacentsJumpingLand(PathList& pathList, const iPoint destination);

	// -----------
	int g = 0;
	int h;
	iPoint pos;
	int pos_z;
	PathNode* parent; // needed to reconstruct the path in the end
	PathNode* parent_z;
	int jumpLength = 0;
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	const ListItem<PathNode>* Find(const iPoint& point) const;

	int FindCounter(const iPoint& point);
	void CreateRepeteadsList(PathList& list, const iPoint node, int& numRepeteads);

	// Returns the Pathnode with lowest score in this list or NULL if empty
	ListItem<PathNode>* GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	List<PathNode> list;
};

class PathFinding : public Module
{
public:

	PathFinding();

	// Destructor
	~PathFinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B
	int CreatePath(const iPoint& origin, const iPoint& destination);
	int CreatePathLand(const iPoint& origin, const iPoint& destination);
	//int CreateJumpingLandPath(const iPoint& origin, const iPoint& destination, int characterTileWidth, int characterTileHeight, int maxCharacterTilesJump);

	// To request all tiles involved in the last generated path
	const DynArray<iPoint>* GetLastPath() const;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	static int multiThreadCreatePath(void* data);

	void CombineTwoRepeteadsList(const PathList& list1, const PathList& list2, PathList& result);

private:

	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	uchar* map;
	// we store the created path here
	DynArray<iPoint> last_path;

};

#endif // __PATHFINDING_H__