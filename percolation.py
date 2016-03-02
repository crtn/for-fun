#!/usr/bin/env python
# -*- coding: utf-8 -*-
from scipy import stats
from wsgiref.simple_server import make_server

magicNumber = 0.592


def element(tag, attrs=""):
    def wrapper(func):
        def call(*args, **kwargs):
            return "<{0}{1}>{2}</{0}>".format(tag, attrs, func(*args, **kwargs))
        return call
    return wrapper


@element("table")
def table(tbl):
    @element("tr")
    def tr(row):
        @element("td", ' class="trans"')
        def td_trans(d):
            return "{}".format(d)

        @element("td", ' class="block"')
        def td_block(d):
            return "{}".format(d)

        @element("td", ' class="upper"')
        def td_upper(d):
            return "{}".format(d)

        @element("td", ' class="bottom"')
        def td_bottom(d):
            return "{}".format(d)

        callbacks = {0: td_block, 1: td_trans, 2: td_upper, 3: td_bottom}
        symbols = {0: '+', 1: '+', 2: '+', 3: '+'}
        return ''.join(map(lambda pack: pack[0](pack[1]),
                           zip(map(callbacks.get, row), map(symbols.get, row))))

    return '\n'.join(map(tr, tbl))


def generate(prob, nRow, nCol):
    dist = stats.bernoulli(prob)
    matrix = dist.rvs((nRow + 2, nCol))
    matrix[0, :] = 1
    matrix[nRow + 1, :] = 1
    return matrix


class UnionFind(object):
    def __init__(self, nRow, nCol):
        self.nRow = nRow
        self.nCol = nCol
        self.parents = list(range(nRow * nCol))

    def find(self, i, j):
        s = r = i * self.nCol + j
        while self.parents[r] != r:
            r = self.parents[r]
        while s != r:
            tmp = self.parents[s]
            self.parents[s] = r
            s = tmp
        return r

    def unite(self, i, j, m, n):
        x = self.find(i, j)
        y = self.find(m, n)
        if x != y:
            self.parents[x] = y


def percolation(matrix):
    nRow, nCol = matrix.shape
    uf = UnionFind(nRow, nCol)
    for i in range(nRow - 1):
        for j in range(nCol - 1):
            if matrix[i, j] and matrix[i, j + 1]:
                uf.unite(i, j, i, j + 1)
            if matrix[i, j] and matrix[i + 1, j]:
                uf.unite(i, j, i + 1, j)
    for i in range(nRow - 1):
        if matrix[i, nCol - 1] and matrix[i + 1, nCol - 1]:
            uf.unite(i, nCol - 1, i + 1, nCol - 1)
    for j in range(nCol - 1):
        if matrix[nRow - 1, j] and matrix[nRow - 1, j + 1]:
            uf.unite(nRow - 1, j, nRow - 1, j + 1)

    x = uf.find(0, 0)
    y = uf.find(nRow - 1, 0)

    for i in range(nRow):
        for j in range(nCol):
            if uf.find(i, j) == x:
                matrix[i, j] = 2
    if x != y:
        for i in range(nRow):
            for j in range(nCol):
                if uf.find(i, j) == y:
                    matrix[i, j] = 3

    return x == y


def simulate():
    matrix = generate(magicNumber, 100, 100)
    connected = percolation(matrix)
    txt = table(matrix)
    css = """td.trans { background-color: #00FF00; }
             td.block { background-color: #FF4000; }
             td.upper { background-color: #2E64FE; }
             td.bottom { background-color: #2EFEF7; }
             td { font-size: 50%; }"""
    html = "<html><head><style>" + css \
           + "</style></head><body>" \
           + "<b>percolation: {}</b><br><br>".format(connected) \
           + txt + "</body></html>"
    return html, connected


def __main__():
    def app(env, rep):
        html, _ = simulate()
        rep('200 OK', [('Content-type', 'text/html')])
        return [html.encode('utf-8'), ]

    http_server = make_server('0.0.0.0', 9999, app)
    http_server.serve_forever()


if __name__ == '__main__':
    __main__()
