import struct

def PopCount(b):
    return bin(b).count('1')

class Puzzle:
    def __init__(self):
        self.P = 0
        self.O = 0
    def __init__(self, P, O):
        self.P = P
        self.O = O
    def __init__(self, pack):
        self.P = pack[0]
        self.O = pack[1]
    def EmptyCount(self):
        return 64 - PopCount(self.P | self.O)

class PuzzleScore:
    __DEFAULT_SCORE = -99

    def __init__(self):
        self.P = 0
        self.O = 0
        self.score = __DEFAULT_SCORE
    def __init__(self, P, O, score):
        self.P = P
        self.O = O
        self.score = score
    def __init__(self, pack):
        self.P = pack[0]
        self.O = pack[1]
        self.score = pack[2]
    def EmptyCount(self):
        return 64 - PopCount(self.P | self.O)
    def IsSolved(self):
        return self.score != self.__DEFAULT_SCORE

#TODO: Add PuzzleScoreDepth
        
class PuzzleAllDepthScore:
    __DEFAULT_SCORE = -99

    def __init__(self):
        self.P = 0
        self.O = 0
        self.score = [__DEFAULT_SCORE] * 61
    def __init__(self, P, O, score):
        self.P = P
        self.O = O
        self.score = score
    def __init__(self, pack):
        self.P = pack[0]
        self.O = pack[1]
        self.score = pack[2:]
    def EmptyCount(self):
        return 64 - PopCount(self.P | self.O)
    def IsSolved(self):
        for i in range(0, self.EmptyCount()+1):
            if self.score[i] == self.__DEFAULT_SCORE:
                return False
        return True

# TODO: Remove!
#def Deserialize_Puzzle(file):
#    id_byte = file.read(1)
#    if id_byte == b'\x01':
#        memory_layout = '<QQ'
#        return Puzzle(struct.unpack(memoy_layout, file.read(struct.calcsize(memory_layout))))
#    if id_byte == b'\x02':
#        memory_layout = '<QQb'
#        return PuzzleScore(struct.unpack(memory_layout, file.read(struct.calcsize(memory_layout))))
#    if id_byte == b'\x04':
#        memory_layout = '<QQbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'
#        return PuzzleAllDepthScore(struct.unpack(memory_layout, file.read(struct.calcsize(memory_layout))))
#    return None

def Deserialize_Puzzles(filename):
    with open(filename, "rb") as file:
        while True:
            id_byte = file.read(1)
            if id_byte == b'':
                break
            if id_byte == b'\x01':
                memory_layout = '<QQ'
                yield Puzzle(struct.unpack(memoy_layout, file.read(struct.calcsize(memory_layout))))
            if id_byte == b'\x02':
                memory_layout = '<QQb'
                yield PuzzleScore(struct.unpack(memory_layout, file.read(struct.calcsize(memory_layout))))
            if id_byte == b'\x04':
                memory_layout = '<QQbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'
                yield PuzzleAllDepthScore(struct.unpack(memory_layout, file.read(struct.calcsize(memory_layout))))