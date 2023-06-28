
from __future__ import annotations
import struct
import cv2
import numpy as np
from collections import deque


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

    STARTING_POINT = (len(maze) - 5, len(maze[0]) - 5)
    END_POINT = (5, 5)

    maze[STARTING_POINT[0]][STARTING_POINT[1]] = FREE
    maze[END_POINT[0]][END_POINT[1]] = FREE

    solve_bfs(maze, STARTING_POINT, END_POINT)


if __name__ == "__main__":
    main()
