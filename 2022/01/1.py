import sys
import more_itertools as it


class Elf:
    calories = []

    def __init__(self, calories):
        self.calories = calories


with open(sys.argv[1]) as f:
    elfs = []
    current_calories = []

    for line in f.readlines():
        if line.strip() == "":
            elfs.append(Elf(current_calories))
            current_calories = []
        else:
            calories = int(line.strip())
            current_calories.append(calories)
    # append last elf
    elfs.append(Elf(current_calories))

    print("most calories: ", max(map(lambda e: sum(e.calories), elfs)))
    print(
        "top 3 total: ",
        sum(it.take(3, reversed(sorted(map(lambda e: sum(e.calories),
                                           elfs))))))
