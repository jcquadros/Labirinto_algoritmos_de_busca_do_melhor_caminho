
from __future__ import annotations
import struct
import cv2
import numpy as np
from collections import deque
from multiprocessing import Process


"""
MAZE_HEIGHT = 25
MAZE_WIDTH = 40
OBSTACLE_PROBABILITY = 0.4
"""
CELL_SIZE = 20
OBSTACLE = 1
FREE = 0


class Node:
    def __init__(self, pos, parent=None):
        self.pos = pos
        self.parent = parent
        self.g = 0
        if self.parent is not None:
            d_parent = ((pos[0] - parent.pos[0]) **
                        2 + (pos[1] - parent.pos[1]) ** 2) ** 0.5
            self.g += self.parent.g + d_parent

    def __eq__(self, other: Node) -> bool:
        return self.pos == other.pos

    def __hash__(self):
        return hash(self.pos)


class HeapNode:
    def __init__(self, value, priority):
        self.value = value
        self.priority = priority

    def __lt__(self, other):
        return self.priority < other.priority

    def __eq__(self, other):
        return (self.value == other.value)

    def __hash__(self):
        return hash(self.value)


class Heap:
    def __init__(self):
        self._heap = []
        self._indices = {}

    def __iter__(self):
        return iter(self._heap)

    def __len__(self):
        return len(self._heap)

    def push(self, value, priority):
        if value in self._indices:
            self._update(value, priority)
            return

        self._heap.append(HeapNode(value, priority))
        idx = len(self._heap) - 1
        self._indices[value] = idx
        self._heapify_up(idx)

    def pop(self):
        self._heap[0], self._heap[-1] = self._heap[-1], self._heap[0]
        node = self._heap.pop()
        val, priority = node.value, node.priority

        del self._indices[val]

        if len(self._heap) > 0:
            self._indices[self._heap[0].value] = 0
            self._heapify_down(0)

        return val, priority

    def get(self, val):
        if val not in self._indices:
            return None

        node = self._heap[self._indices[val]]
        val = node.value
        priority = node.priority

        return val, priority

    def _update(self, value, new_priority):
        curr_idx = self._indices[value]
        prev_priority = self._heap[curr_idx].priority
        self._heap[curr_idx].priority = new_priority

        if new_priority < prev_priority:
            self._heapify_up(curr_idx)
        else:
            self._heapify_down(curr_idx)

    def _heapify_up(self, idx):
        parent_idx = (idx - 1) // 2

        while (parent_idx >= 0) and (self._heap[parent_idx].priority > self._heap[idx].priority):
            self._heap[parent_idx], self._heap[idx] = self._heap[idx], self._heap[parent_idx]

            self._indices[self._heap[parent_idx].value] = parent_idx
            self._indices[self._heap[idx].value] = idx

            idx = parent_idx
            parent_idx = (idx - 1) // 2

    def _heapify_down(self, idx):
        left_child_idx = 2 * idx + 1

        while left_child_idx < len(self._heap):
            min_idx = idx

            if self._heap[left_child_idx].priority < self._heap[idx].priority:
                min_idx = left_child_idx

            right_child_idx = left_child_idx + 1

            if (right_child_idx < len(self._heap)) and (self._heap[right_child_idx].priority < self._heap[min_idx].priority):
                min_idx = right_child_idx

            if min_idx == idx:
                break

            self._heap[idx], self._heap[min_idx] = self._heap[min_idx], self._heap[idx]
            self._indices[self._heap[idx].value] = idx
            self._indices[self._heap[min_idx].value] = min_idx

            idx = min_idx
            left_child_idx = 2 * idx + 1


def test_heap():
    h = Heap()

    # test 1: insere de 0 a 10 e deve exibir de 10 a 0
    for i in range(11):
        h.push(i, i)

    """
    print(h._heap)
    for i, node in enumerate(h._heap):
        print(i, ":", (node.value, node.priority))

    print(h._indices)

    return
    """

    while (len(h)) > 0:
        print(h.pop(), end=" ")
    print()

    # test 2: insere de 10 a 0 e deve exibir de 10 a 0
    for i in range(10, -1, -1):
        h.push(i, i)

    while (len(h)) > 0:
        print(h.pop(), end=" ")
    print()

    # test 3: insere de 0 a 10, depois inverte as prioridades. Deve exibir de 0 a 10.
    for i in range(11):
        h.push(i, i)

    for i in range(11):
        h.push(i, 10 - i)

    while (len(h)) > 0:
        print(h.pop(), end=" ")
    print()

    # test 4: idem, mas na outra direcao
    for i in range(11):
        h.push(i, i)

    for i in range(10, -1, -1):
        h.push(i, 10 - i)

    while (len(h)) > 0:
        print(h.pop(), end=" ")
    print()


def generate_maze(height, width, obstacle_probability):
    maze = np.zeros((height, width))

    for i in range(len(maze)):
        for j in range(len(maze[0])):
            if np.random.random() < obstacle_probability:
                maze[i][j] = OBSTACLE

    return maze


def draw_cell(view, i, j, color=(0, 0, 0)):
    # draw content
    view = cv2.rectangle(view,
                         (j * CELL_SIZE, i * CELL_SIZE),
                         ((j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE),
                         color, -1)

    # draw border
    view = cv2.rectangle(view,
                         (j * CELL_SIZE, i * CELL_SIZE),
                         ((j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE),
                         (0, 0, 0), 2)
    return view


def draw_maze(maze):
    view = np.zeros((len(maze) * CELL_SIZE,
                     len(maze[0]) * CELL_SIZE, 3)) + 255

    for i in range(len(maze)):
        for j in range(len(maze[0])):
            color = (255, 255, 255)
            if maze[i][j] == OBSTACLE:
                color = (0, 0, 0)
            view = draw_cell(view, i, j, color)

    return view


def draw_solution(maze, starting_point, end_point, expanded, to_expand, expanded_node, path=[], wait=1):
    view = draw_maze(maze)

    for node in expanded:
        view = draw_cell(view, node[0], node[1], (255, 0, 0))

    for node in to_expand:
        if type(node) == HeapNode:
            node = node.value

        if type(node) == Node:
            node = node.pos

        view = draw_cell(view, node[0], node[1], (0, 255, 255))

    view = draw_cell(
        view, starting_point[0], starting_point[1], (0, 0, 255))
    view = draw_cell(view, end_point[0], end_point[1], (0, 255, 0))
    view = draw_cell(view, expanded_node[0], expanded_node[1], (255, 0, 255))

    for node in path:
        view = draw_cell(view, node[0], node[1], (255, 0, 255))

    cv2.imshow("maze", view)
    cv2.waitKey(wait)


def expand_node(node, maze, to_expand, expanded):
    neighbours = [
        [1, 0],
        [1, 1],
        [0, 1],
        [-1, 1],
        [-1, 0],
        [-1, -1],
        [0, -1],
        [1, -1],
    ]

    for dy, dx in neighbours:
        i = node.pos[0] + dy
        j = node.pos[1] + dx
        if (i < 0) or (i >= len(maze)):
            continue
        if (j < 0) or (j >= len(maze[0])):
            continue
        new_node = Node((i, j), parent=node)
        if (maze[i][j] == OBSTACLE) or ((i, j) == node.pos) or (new_node.pos in expanded) or (new_node in to_expand):
            continue
        to_expand.append(new_node)
    expanded.append(node.pos)


def solve_dfs(maze, starting_point, end_point):
    to_expand = [Node(starting_point)]
    expanded = []
    succeded = False

    draw_solution(maze, starting_point, end_point,
                  expanded, to_expand, starting_point, wait=0)

    while len(to_expand) > 0:
        node = to_expand.pop()

        if (node.pos == end_point):
            succeded = True
            expanded.append(node.pos)
            final_node = node
            break

        expand_node(node, maze, to_expand, expanded)

        draw_solution(maze, starting_point, end_point,
                      expanded, to_expand, node.pos)

    path = []
    if succeded:
        path_node = final_node
        while path_node is not None:
            path.append(path_node.pos)
            path_node = path_node.parent
        path.reverse()

    draw_solution(maze, starting_point, end_point,
                  [], [], node.pos, path=path, wait=0)

    return path, succeded


def solve_bfs(maze, starting_point, end_point):
    to_expand = deque()
    to_expand.append(Node(starting_point))
    expanded = []
    succeded = False

    draw_solution(maze, starting_point, end_point,
                  expanded, to_expand, starting_point, wait=0)

    while len(to_expand) > 0:
        node = to_expand.popleft()

        if (node.pos == end_point):
            succeded = True
            expanded.append(node.pos)
            final_node = node
            break

        expand_node(node, maze, to_expand, expanded)

        draw_solution(maze, starting_point, end_point,
                      expanded, to_expand, node.pos)

    path = []
    if succeded:
        path_node = final_node
        while path_node is not None:
            path.append(path_node.pos)
            path_node = path_node.parent
        path.reverse()

    draw_solution(maze, starting_point, end_point,
                  [], [], node.pos, path=path, wait=0)

    return path, succeded


def expand_node_a_star(node, maze, to_expand, expanded, end_point):
    for i in range(node.pos[0]-1, node.pos[0]+2):
        if (i < 0) or (i >= len(maze)):
            continue
        for j in range(node.pos[1]-1, node.pos[1]+2):
            if (j < 0) or (j >= len(maze[0])):
                continue

            if (maze[i][j] == OBSTACLE) or ((i, j) == node.pos) or ((i, j) in expanded):
                continue

            new_node = Node((i, j), node)
            h = np.sqrt((i - end_point[0])**2 + (j - end_point[1])**2)

            data = to_expand.get(new_node)
            if data is not None:
                stored_node, _ = data
                if new_node.g < stored_node.g:
                    to_expand.push(new_node, new_node.g + h)
            else:
                to_expand.push(new_node, new_node.g + h)

    expanded.add(node.pos)


def solve_a_star(maze, starting_point, end_point):
    path = [starting_point]
    to_expand = Heap()

    dist_goal = np.sqrt(
        (starting_point[0] - end_point[0]) ** 2 +
        (starting_point[1] - end_point[1]) ** 2
    )

    to_expand.push(Node(starting_point), dist_goal)

    expanded = set()
    succeded = False

    draw_solution(maze, starting_point, end_point,
                  expanded, to_expand, starting_point, wait=0)

    while len(to_expand) > 0:
        node, _ = to_expand.pop()

        if (node.pos == end_point):
            succeded = True
            expanded.add(node.pos)
            final_node = node
            break

        expand_node_a_star(node, maze, to_expand, expanded, end_point)

        draw_solution(maze, starting_point, end_point,
                      expanded, to_expand, node.pos, wait=1)

    path = []
    if succeded:
        path_node = final_node
        while path_node is not None:
            path.append(path_node.pos)
            path_node = path_node.parent
        path.reverse()

    draw_solution(maze, starting_point, end_point,
                  [], [], node.pos, path=path, wait=0)

    return path, succeded


def load_maze(input_path):
    with open(input_path, "rb") as f:
        # Lendo as dimensões do labirinto
        height = struct.unpack('i', f.read(4))[0]
        width = struct.unpack('i', f.read(4))[0]

        # Inicializando o labirinto
        maze = []
        for _ in range(height):
            row = []
            for _ in range(width):
                # Lendo cada célula do labirinto
                cell = struct.unpack('B', f.read(1))[0]
                row.append(cell)
            maze.append(row)

    return maze


def main():
    #maze = load_maze("dados/labirintos/maze10x10_obstaculos_1.bin")
    #maze = load_maze("dados/labirintos/maze30x30_livre.bin")
    maze = load_maze("dados/labirintos/maze30x30_obstaculos_1.bin")

    #STARTING_POINT = (len(maze) - 1, 0)
    STARTING_POINT = (len(maze) - 5, len(maze[0]) - 5)
    END_POINT = (0, len(maze[0]) - 1)
    # END_POINT = (0, 0)
    #END_POINT = (len(maze) - 1, len(maze[0]) - 1)
    #END_POINT = (0, len(maze[0]) - 1)
    #END_POINT = (5, 5)

    maze[STARTING_POINT[0]][STARTING_POINT[1]] = FREE
    maze[END_POINT[0]][END_POINT[1]] = FREE

    #solve_a_star(maze, STARTING_POINT, END_POINT)
    solve_dfs(maze, STARTING_POINT, END_POINT)
    # solve_bfs(maze, STARTING_POINT, END_POINT)

    return
    p1 = Process(target=solve_dfs, args=(maze, STARTING_POINT, END_POINT))
    p2 = Process(target=solve_bfs, args=(maze, STARTING_POINT, END_POINT))
    p3 = Process(target=solve_a_star, args=(maze, STARTING_POINT, END_POINT))

    p1.start()
    p2.start()
    p3.start()

    p1.join()
    p2.join()
    p3.join()


if __name__ == "__main__":
    main()
