//
// Created by Mark Pedersen on 2018-02-20.
//

#ifndef JUMP_POINT_SEARCH_H
#define JUMP_POINT_SEARCH_H

#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>

#ifdef _DEBUG
#include <cassert>
#define JPS_ASSERT(cond) assert(cond)
#else
#define JPS_ASSERT(cond)
#endif


namespace JPS {

    enum Result {
        NO_PATH,
        FOUND_PATH,
        NEED_MORE_STEPS,
        EMPTY_PATH
    };

    struct Position {
        unsigned x, y;

        inline bool operator==(const Position &p) const {
            return x == p.x && y == p.y;
        }

        inline bool operator!=(const Position &p) const {
            return x != p.x || y != p.y;
        }

        // for sorting
        inline bool operator<(const Position &p) const {
            return y < p.y || (y == p.y && x < p.x);
        }

        inline bool isValid() const { return x != unsigned(-1); }
    };

    typedef std::vector<Position> PathVector;

    inline static Position Pos(unsigned x, unsigned y) {
        Position p;
        p.x = x;
        p.y = y;
        return p;
    }

    namespace Internal {

        static const Position npos = Pos(-1, -1);

        class Node {
        public:
            Node(const Position &p) : f(0), g(0), pos(p), parent(0), flags(0) {}

            unsigned f, g;
            const Position pos;
            const Node *parent;

            inline void setOpen() { flags |= 1; }

            inline void setClosed() { flags |= 2; }

            inline unsigned isOpen() const { return flags & 1; }

            inline unsigned isClosed() const { return flags & 2; }

            inline void clearState() {
                f = 0;
                g = 0, parent = 0;
                flags = 0;
            }

        private:
            unsigned flags;
        };
    } // end namespace Internal

    namespace Heuristic {
        inline unsigned Manhattan(const Internal::Node *a, const Internal::Node *b) {
            return abs(int(a->pos.x - b->pos.x)) + abs(int(a->pos.y - b->pos.y));
        }

        inline unsigned Euclidean(const Internal::Node *a, const Internal::Node *b) {
            float fx = float(int(a->pos.x - b->pos.x));
            float fy = float(int(a->pos.y - b->pos.y));
            return unsigned(int(sqrtf(fx * fx + fy * fy)));
        }
    } // end namespace heuristic

    namespace Internal {

        typedef std::vector<Node *> NodeVector;

        class OpenList {
        public:
            inline void push(Node *node) {
                JPS_ASSERT(node);
                nodes.push_back(node);
                std::push_heap(nodes.begin(), nodes.end(), _compare);
            }

            inline Node *pop() {
                std::pop_heap(nodes.begin(), nodes.end(), _compare);
                Node *node = nodes.back();
                nodes.pop_back();
                return node;
            }

            inline bool empty() const {
                return nodes.empty();
            }

            inline void clear() {
                nodes.clear();
            }

            inline void fixup() {
                std::make_heap(nodes.begin(), nodes.end(), _compare);
            }

        protected:
            static inline bool _compare(const Node *a, const Node *b) {
                return a->f > b->f;
            }

            NodeVector nodes;
        };

        template<typename GRID>
        class Searcher {
        public:
            Searcher(const GRID &g)
                    : grid(g), endNode(NULL), skip(1), stepsRemain(0), stepsDone(0) {}

            // single-call
            bool findPath(PathVector &path, Position start, Position end, unsigned step);

            // incremental pathfinding
            Result findPathInit(Position start, Position end);

            Result findPathStep(int limit);

            bool findPathFinish(PathVector &path, unsigned step);

            // misc
            void freeMemory();

            inline void setSkip(int s) { skip = std::max(1, s); }

            inline size_t getStepsDone() const { return stepsDone; }

            inline size_t getNodesExpanded() const { return nodegrid.size(); }

        private:

            typedef std::map<Position, Node> NodeGrid;

            const GRID &grid;
            Node *endNode;
            int skip;
            int stepsRemain;
            size_t stepsDone;
            OpenList open;

            NodeGrid nodegrid;

            Node *getNode(const Position &pos);

            void identifySuccessors(const Node *n);

            bool generatePath(PathVector &path, unsigned step) const;

#ifndef JPS_DISABLE_GREEDY

            bool findPathGreedy(Node *start);

#endif

#ifdef JPS_ASTAR_ONLY
            unsigned findNeighborsAStar(const Node *n, Position *wptr);
#else

            unsigned findNeighbors(const Node *n, Position *wptr) const;

            Position jumpP(const Position &p, const Position &src);

            Position jumpD(Position p, int dx, int dy);

            Position jumpX(Position p, int dx);

            Position jumpY(Position p, int dy);

#endif
        };

        template<typename GRID>
        inline Node *Searcher<GRID>::getNode(const Position &pos) {
            JPS_ASSERT(grid(pos.x, pos.y));
            return &nodegrid.insert(std::make_pair(pos, Node(pos))).first->second;
        }

#ifndef JPS_ASTAR_ONLY

        template<typename GRID>
        Position Searcher<GRID>::jumpP(const Position &p, const Position &src) {
            JPS_ASSERT(grid(p.x, p.y));

            int dx = int(p.x - src.x);
            int dy = int(p.y - src.y);
            JPS_ASSERT(dx || dy);

            if (dx && dy)
                return jumpD(p, dx, dy);
            else if (dx)
                return jumpX(p, dx);
            else if (dy)
                return jumpY(p, dy);

            // not reached
            JPS_ASSERT(false);
            return npos;
        }

        template<typename GRID>
        Position Searcher<GRID>::jumpD(Position p, int dx, int dy) {
            JPS_ASSERT(grid(p.x, p.y));
            JPS_ASSERT(dx && dy);

            const Position &endpos = endNode->pos;
            int steps = 0;

            while (true) {
                if (p == endpos)
                    break;

                ++steps;
                const unsigned x = p.x;
                const unsigned y = p.y;

                if ((grid(x - dx, y + dy) && !grid(x - dx, y)) || (grid(x + dx, y - dy) && !grid(x, y - dy)))
                    break;

                const bool gdx = grid(x + dx, y);
                const bool gdy = grid(x, y + dy);

                if (gdx && jumpX(Pos(x + dx, y), dx).isValid())
                    break;

                if (gdy && jumpY(Pos(x, y + dy), dy).isValid())
                    break;

                if ((gdx || gdy) && grid(x + dx, y + dy)) {
                    p.x += dx;
                    p.y += dy;
                } else {
                    p = npos;
                    break;
                }
            }
            stepsDone += (unsigned) steps;
            stepsRemain -= steps;
            return p;
        }

        template<typename GRID>
        inline Position Searcher<GRID>::jumpX(Position p, int dx) {
            JPS_ASSERT(dx);
            JPS_ASSERT(grid(p.x, p.y));

            const unsigned y = p.y;
            const Position &endpos = endNode->pos;
            const int skip = this->skip;
            int steps = 0;

            unsigned a = ~((!!grid(p.x, y + skip)) | ((!!grid(p.x, y - skip)) << 1));

            while (true) {
                const unsigned xx = p.x + dx;
                const unsigned b = (!!grid(xx, y + skip)) | ((!!grid(xx, y - skip)) << 1);

                if ((b & a) || p == endpos)
                    break;
                if (!grid(xx, y)) {
                    p = npos;
                    break;
                }

                p.x += dx;
                a = ~b;
                ++steps;
            }

            stepsDone += (unsigned) steps;
            stepsRemain -= steps;
            return p;
        }

        template<typename GRID>
        inline Position Searcher<GRID>::jumpY(Position p, int dy) {
            JPS_ASSERT(dy);
            JPS_ASSERT(grid(p.x, p.y));

            const unsigned x = p.x;
            const Position &endpos = endNode->pos;
            const int skip = this->skip;
            int steps = 0;

            unsigned a = ~((!!grid(x + skip, p.y)) | ((!!grid(x - skip, p.y)) << 1));

            while (true) {
                const unsigned yy = p.y + dy;
                const unsigned b = (!!grid(x + skip, yy)) | ((!!grid(x - skip, yy)) << 1);

                if ((a & b) || p == endpos)
                    break;
                if (!grid(x, yy)) {
                    p = npos;
                    break;
                }

                p.y += dy;
                a = ~b;
            }

            stepsDone += (unsigned) steps;
            stepsRemain -= steps;
            return p;
        }

#endif // JPS_ASTAR_ONLY

#define JPS_CHECKGRID(dx, dy) (grid(x+(dx), y+(dy)))
#define JPS_ADDPOS(dx, dy)    do { *w++ = Pos(x+(dx), y+(dy)); } while(0)
#define JPS_ADDPOS_CHECK(dx, dy) do { if(JPS_CHECKGRID(dx, dy)) JPS_ADDPOS(dx, dy); } while(0)
#define JPS_ADDPOS_NO_TUNNEL(dx, dy) do { if(grid(x+(dx),y) || grid(x,y+(dy))) JPS_ADDPOS_CHECK(dx, dy); } while(0)

#ifndef JPS_ASTAR_ONLY

        template<typename GRID>
        unsigned Searcher<GRID>::findNeighbors(const Node *n, Position *wptr) const {
            Position *w = wptr;
            const unsigned x = n->pos.x;
            const unsigned y = n->pos.y;
            const int skip = this->skip;

            if (!n->parent) {
                // straight moves
                JPS_ADDPOS_CHECK(-skip, 0);
                JPS_ADDPOS_CHECK(0, -skip);
                JPS_ADDPOS_CHECK(0, skip);
                JPS_ADDPOS_CHECK(skip, 0);

                // diagonal moves + prevent tunneling
                JPS_ADDPOS_NO_TUNNEL(-skip, -skip);
                JPS_ADDPOS_NO_TUNNEL(-skip, skip);
                JPS_ADDPOS_NO_TUNNEL(skip, -skip);
                JPS_ADDPOS_NO_TUNNEL(skip, skip);

                return unsigned(w - wptr);
            }

            // jump directions (both -1, 0, or 1)
            int dx = int(x - n->parent->pos.x);
            dx /= std::max(abs(dx), 1);
            dx *= skip;
            int dy = int(y - n->parent->pos.y);
            dy /= std::max(abs(dy), 1);
            dy *= skip;

            if (dx && dy) {
                // diagonal
                // natural neighbors
                bool walkX = false;
                bool walkY = false;
                if ((walkX = grid(x + dx, y)))
                    *w++ = Pos(x + dx, y);
                if ((walkY = grid(x, y + dy)))
                    *w++ = Pos(x, y + dy);

                if (walkX || walkY)
                    JPS_ADDPOS_CHECK(dx, dy);

                // forced neighbors
                if (walkY && !JPS_CHECKGRID(-dx, 0))
                    JPS_ADDPOS_CHECK(-dx, dy);

                if (walkX && !JPS_CHECKGRID(0, -dy))
                    JPS_ADDPOS_CHECK(dx, -dy);
            } else if (dx) {
                // along X axis
                if (JPS_CHECKGRID(dx, 0)) {
                    JPS_ADDPOS(dx, 0);

                    // Forced neighbors (+ prevent tunneling)
                    if (!JPS_CHECKGRID(0, skip))
                        JPS_ADDPOS_CHECK(dx, skip);
                    if (!JPS_CHECKGRID(0, -skip))
                        JPS_ADDPOS_CHECK(dx, -skip);
                }


            } else if (dy) {
                // along Y axis
                if (JPS_CHECKGRID(0, dy)) {
                    JPS_ADDPOS(0, dy);

                    // Forced neighbors (+ prevent tunneling)
                    if (!JPS_CHECKGRID(skip, 0))
                        JPS_ADDPOS_CHECK(skip, dy);
                    if (!JPS_CHECKGRID(-skip, 0))
                        JPS_ADDPOS_CHECK(-skip, dy);
                }
            }

            return unsigned(w - wptr);
        }

#else
        template <typename GRID> unsigned Searcher<GRID>::findNeighborsAStar(const Node *n, Position *wptr)
        {
            Position *w = wptr;
            const int x = n->pos.x;
            const int y = n->pos.y;
            const int d = skip;
            JPS_ADDPOS_NO_TUNNEL(-d, -d);
            JPS_ADDPOS_CHECK    ( 0, -d);
            JPS_ADDPOS_NO_TUNNEL(+d, -d);
            JPS_ADDPOS_CHECK    (-d,  0);
            JPS_ADDPOS_CHECK    (+d,  0);
            JPS_ADDPOS_NO_TUNNEL(-d, +d);
            JPS_ADDPOS_CHECK    ( 0, +d);
            JPS_ADDPOS_NO_TUNNEL(+d, +d);
            stepsDone += 8;
            return unsigned(w - wptr);
        }
#endif // JPS_ASTAR_ONLY
//-------------------------------------------------
#undef JPS_ADDPOS
#undef JPS_ADDPOS_CHECK
#undef JPS_ADDPOS_NO_TUNNEL
#undef JPS_CHECKGRID


        template<typename GRID>
        void Searcher<GRID>::identifySuccessors(const Node *n) {
            Position buf[8];
#ifdef JPS_ASTAR_ONLY
            const int num = findNeighborsAStar(n, &buf[0]);
#else
            const int num = findNeighbors(n, &buf[0]);
#endif
            for (int i = num - 1; i >= 0; --i) {
                // Invariant: A node is only a valid neighbor if the corresponding grid position is walkable (asserted in jumpP)
#ifdef JPS_ASTAR_ONLY
                Position jp = buf[i];
#else
                Position jp = jumpP(buf[i], n->pos);
                if (!jp.isValid())
                    continue;
#endif
                // Now that the grid position is definitely a valid jump point, we have to create the actual node.
                Node *jn = getNode(jp);
                JPS_ASSERT(jn && jn != n);
                if (!jn->isClosed()) {
                    unsigned extraG = Heuristic::Euclidean(jn, n);
                    unsigned newG = n->g + extraG;
                    if (!jn->isOpen() || newG < jn->g) {
                        jn->g = newG;
                        jn->f = jn->g + Heuristic::Manhattan(jn, endNode);
                        jn->parent = n;
                        if (!jn->isOpen()) {
                            open.push(jn);
                            jn->setOpen();
                        } else
                            open.fixup();
                    }
                }
            }
        }

        template<typename GRID>
        bool Searcher<GRID>::generatePath(PathVector &path, unsigned step) const {
            if (!endNode)
                return false;
            size_t offset = path.size();
            if (step) {
                const Node *next = endNode;
                const Node *prev = endNode->parent;
                if (!prev)
                    return false;
                do {
                    const unsigned x = next->pos.x, y = next->pos.y;
                    int dx = int(prev->pos.x - x);
                    int dy = int(prev->pos.y - y);
                    JPS_ASSERT(!dx || !dy || abs(dx) == abs(dy)); // known to be straight, if diagonal
                    const int steps = std::max(abs(dx), abs(dy));
                    dx /= std::max(abs(dx), 1);
                    dy /= std::max(abs(dy), 1);
                    dx *= int(step);
                    dy *= int(step);
                    int dxa = 0, dya = 0;
                    for (int i = 0; i < steps; i += step) {
                        path.push_back(Pos(x + dxa, y + dya));
                        dxa += dx;
                        dya += dy;
                    }
                    next = prev;
                    prev = prev->parent;
                } while (prev);
            } else {
                const Node *next = endNode;
                if (!next->parent)
                    return false;
                do {
                    JPS_ASSERT(next != next->parent);
                    path.push_back(next->pos);
                    next = next->parent;
                } while (next->parent);
            }
            std::reverse(path.begin() + offset, path.end());
            return true;
        }

        template<typename GRID>
        bool Searcher<GRID>::findPath(PathVector &path, Position start, Position end, unsigned step) {
            printf("running findpath");
            Result res = findPathInit(start, end);

            // If this is true, the resulting path is empty (findPathFinish() would fail, so this needs to be checked before)
            if (res == EMPTY_PATH)
                return true;

            while (true) {
                switch (res) {
                    case NEED_MORE_STEPS:
                        res = findPathStep(0);
                        break; // the switch

                    case FOUND_PATH:
                        {printf("path is now of size... %lu \n", path.size());
                        return findPathFinish(path, step);}

                    case NO_PATH:
                         printf("no path! path is now of size... %lu \n", path.size());
                    default:
                        return false;
                }
            }
        }

        template<typename GRID>
        Result Searcher<GRID>::findPathInit(Position start, Position end) {
            for (auto it = nodegrid.begin(); it != nodegrid.end(); ++it)
                it->second.clearState();
            open.clear();
            endNode = NULL;
            stepsDone = 0;

            // If skip is > 1, make sure the points are aligned so that the search will always hit them
            start.x = (start.x / skip) * skip;
            start.y = (start.y / skip) * skip;
            end.x = (end.x / skip) * skip;
            end.y = (end.y / skip) * skip;

            if (start == end) {
                // There is only a path if this single position is walkable.
                // But since the starting position is omitted, there is nothing to do here.
                return grid(end.x, end.y) ? EMPTY_PATH : NO_PATH;
            }

            // If start or end point are obstructed, don't even start
            if (!grid(start.x, start.y) || !grid(end.x, end.y))
                return NO_PATH;

            endNode = getNode(end);
            Node *startNode = getNode(start);
            JPS_ASSERT(startNode && endNode);

#ifndef JPS_DISABLE_GREEDY
            // Try the quick way out first
            if (findPathGreedy(startNode))
                return FOUND_PATH;
#endif

            open.push(startNode);

            return NEED_MORE_STEPS;
        }

        template<typename GRID>
        Result Searcher<GRID>::findPathStep(int limit) {
            stepsRemain = limit;
            do {
                if (open.empty())
                    return NO_PATH;
                Node *n = open.pop();
                n->setClosed();
                if (n == endNode)
                    return FOUND_PATH;
                identifySuccessors(n);
            } while (stepsRemain >= 0);
            return NEED_MORE_STEPS;
        }

        template<typename GRID>
        bool Searcher<GRID>::findPathFinish(PathVector &path, unsigned step) {
            return generatePath(path, step);
        }

#ifndef JPS_DISABLE_GREEDY

        template<typename GRID>
        bool Searcher<GRID>::findPathGreedy(Node *n) {
            Position midpos = npos;
            int x = n->pos.x;
            int y = n->pos.y;
            int ex = endNode->pos.x;
            int ey = endNode->pos.y;

            JPS_ASSERT(x != ex || y != ey); // must not be called when start==end
            JPS_ASSERT(n != endNode);

            const int skip = this->skip;

            int dx = int(ex - x);
            int dy = int(ey - y);
            const int adx = abs(dx);
            const int ady = abs(dy);
            dx /= std::max(adx, 1);
            dy /= std::max(ady, 1);
            dx *= skip;
            dy *= skip;

            // go diagonally first
            if (x != ex && y != ey) {
                JPS_ASSERT(dx && dy);
                const int minlen = (int) std::min(adx, ady);
                const int tx = x + dx * minlen;
                for (; x != tx;) {
                    if (grid(x, y) && (grid(x + dx, y) || grid(x, y + dy))) // prevent tunneling as well
                    {
                        x += dx;
                        y += dy;
                    } else
                        return false;
                }

                if (!grid(x, y))
                    return false;

                midpos = Pos(x, y);
            }

            JPS_ASSERT(x == ex || y == ey);

            if (!(x == ex && y == ey)) {
                while (x != ex)
                    if (!grid(x += dx, y))
                        return false;

                while (y != ey)
                    if (!grid(x, y += dy))
                        return false;

                JPS_ASSERT(x == ex && y == ey);
            }

            if (midpos.isValid()) {
                Node *mid = getNode(midpos);
                JPS_ASSERT(mid && mid != n);
                mid->parent = n;
                if (mid != endNode)
                    endNode->parent = mid;
            } else
                endNode->parent = n;

            return true;
        }

#endif

        template<typename GRID>
        void Searcher<GRID>::freeMemory() {
            NodeGrid v;
            nodegrid.swap(v);
            endNode = NULL;
            open.clear();
        }

    }

    using Internal::Searcher;

    template<typename GRID>
    bool findPath(PathVector &path, const GRID &grid, unsigned startx, unsigned starty, unsigned endx, unsigned endy,
                  unsigned step = 0, int skip = 0, // optional params
                  size_t *stepsDone = NULL, size_t *nodesExpanded = NULL // for information
    ) {
        Searcher<GRID> search(grid);
        search.setSkip(skip);
        bool found = search.findPath(path, Pos(startx, starty), Pos(endx, endy), step);
        if (stepsDone)
            *stepsDone = search.getStepsDone();
        if (nodesExpanded)
            *nodesExpanded = search.getNodesExpanded();
        return found;
    }


}

#endif