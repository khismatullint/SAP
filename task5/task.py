import json
import itertools

def _flatten(seq):
    """Разворачивает вложенные списки, сохраняя порядок."""
    for x in seq:
        if isinstance(x, list):
            yield from _flatten(x)
        else:
            yield x

def contradiction_core(rank_json_1: str, rank_json_2: str) -> str:
    """
    Возвращает ядро противоречий двух ранжировок.
    Теперь понимает вложенные списки-«корзины».
    """
    #парсим вход
    try:
        r1_raw = json.loads(rank_json_1)
        r2_raw = json.loads(rank_json_2)
    except json.JSONDecodeError as e:
        return json.dumps({"error": f"Invalid JSON input: {e.msg}"}, ensure_ascii=False)

    #«выравниваем» ранжировки
    r1 = list(_flatten(r1_raw))
    r2 = list(_flatten(r2_raw))

    # проверяем одно и то же множество объектов
    if set(r1) != set(r2):
        return json.dumps({"error": "Rankings must contain the same set of items"}, ensure_ascii=False)

    #позиции элементов
    pos1 = {item: idx for idx, item in enumerate(r1)}
    pos2 = {item: idx for idx, item in enumerate(r2)}

    # строим ядро
    core = []
    for a, b in itertools.combinations(pos1, 2):
        if (pos1[a] - pos1[b]) * (pos2[a] - pos2[b]) < 0:
            core.append(sorted([a, b]))

    core.sort(key=lambda pair: (pair[0], pair[1]))  # детерминированный порядок
    return json.dumps(core, ensure_ascii=False)

# --------------------------------------------------------------------
if __name__ == "__main__":
    json1 = '[1, [2, 3], 4, [5, 6, 7], 8, 9, 10]'
    json2 = '[[1, 2], [3, 4, 5], 6, 7, 9, [8, 10]]'
    print(contradiction_core(json1, json2))
