# task.py  ────────────────────────────────────────────────────────────────
import json, ast
from typing import Dict, List, Tuple

# ─────────────────────────────────────────────────────────────────────────
# safe_load: строгий JSON → json.loads; иначе допускаем '…', запятые и т.п.
# ─────────────────────────────────────────────────────────────────────────
def safe_load(text: str):
    try:
        return json.loads(text)
    except json.JSONDecodeError:
        return ast.literal_eval(text)

# ─────────────────────────────────────────────────────────────────────────
# приводим вход к формату {term: [[x, m], …]}
# ─────────────────────────────────────────────────────────────────────────
def to_simple_dict(raw_json: str) -> Dict[str, List[List[float]]]:
    data = safe_load(raw_json)

    # формат 1: уже словарь term → points
    if all(isinstance(v, list) and v and isinstance(v[0], list)
           for v in data.values()):
        return data

    # формат 2: объект с единственным ключом, внутри массив term-объектов
    term_array = next(iter(data.values()))
    return {term["id"]: term["points"] for term in term_array}

# ─────────────────────────────────────────────────────────────────────────
# μ(x) по ломаной
# ─────────────────────────────────────────────────────────────────────────
def calculate_membership(value: float, points: List[List[float]]) -> float:
    for (x1, y1), (x2, y2) in zip(points, points[1:]):
        if x1 <= value <= x2:
            if x2 == x1:
                return max(y1, y2)
            return y1 + (y2 - y1) * (value - x1) / (x2 - x1)
    return 0.0

# ─────────────────────────────────────────────────────────────────────────
# фаззификация
# ─────────────────────────────────────────────────────────────────────────
def fuzzification(value: float,
                  functions: Dict[str, List[List[float]]]
                 ) -> Dict[str, float]:
    return {term: calculate_membership(value, pts) for term, pts in functions.items()}

# ─────────────────────────────────────────────────────────────────────────
# применение правил (Mamdani, max–min)
# ─────────────────────────────────────────────────────────────────────────
def apply_rule(fuzzy_temp: Dict[str, float],
               rules,                              # dict или list
               heating_terms: Dict[str, List[List[float]]]
              ) -> Dict[str, float]:

    # приводим список пар → словарь, если надо
    if isinstance(rules, list):
        rules = {k: v for k, v in rules}

    # синонимы, чтобы данные «сцепились»
    syn = {"нормально": "комфортно",
           "умеренно":  "умеренный"}

    heat_vals = {t: 0.0 for t in heating_terms}

    for temp_term, heat_term in rules.items():
        temp_term = syn.get(temp_term, temp_term)
        heat_term = syn.get(heat_term, heat_term)

        if temp_term in fuzzy_temp and heat_term in heat_vals:
            heat_vals[heat_term] = max(heat_vals[heat_term], fuzzy_temp[temp_term])
    return heat_vals

# ─────────────────────────────────────────────────────────────────────────
# дефаззификация (центр тяжести треугольника/трапеции)
# ─────────────────────────────────────────────────────────────────────────
def defuzzification(heat_vals: Dict[str, float],
                    heating_terms: Dict[str, List[List[float]]]
                   ) -> float:
    num = den = 0.0
    for term, mu in heat_vals.items():
        if mu == 0:
            continue
        xs = [x for x, _ in heating_terms[term]]
        center = sum(xs) / len(xs)          # геом. центр точек
        num += center * mu
        den += mu
    return num / den if den else 0.0

# ─────────────────────────────────────────────────────────────────────────
# основная функция управления
# ─────────────────────────────────────────────────────────────────────────
def main(temperature_json: str,
         heating_json: str,
         rules_json: str,
         current_temperature: float) -> float:

    temp_funcs = to_simple_dict(temperature_json)
    heat_funcs = to_simple_dict(heating_json)
    rules      = safe_load(rules_json)

    fuzzy_temp   = fuzzification(current_temperature, temp_funcs)
    heating_vals = apply_rule(fuzzy_temp, rules, heat_funcs)
    return defuzzification(heating_vals, heat_funcs)

# ─────────────────────────────────────────────────────────────────────────
# демо-запуск: сначала «простые» строки, затем ваши файлы
# ─────────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    # --- пример ---
    temperature_json = """{
        "холодно":   [[0,1],[16,1],[20,0],[50,0]],
        "комфортно": [[16,0],[20,1],[22,1],[26,0]],
        "жарко":     [[0,0],[22,0],[26,1],[50,1]]
    }"""
    heating_json = """{
        "слабо":      [[0,1],[6,1],[10,0],[20,0]],
        "умеренный":  [[6,0],[10,1],[12,1],[16,0]],
        "интенсивно": [[0,0],[12,0],[16,1],[20,1]]
    }"""
    rules_json = """{
        "холодно":   "интенсивно",
        "комфортно": "умеренный",
        "жарко":     "слабо"
    }"""

    print("Уровень нагрева:", main(temperature_json, heating_json, rules_json, 17))

    try:
        with open("функции-принадлежности-температуры.json", encoding="utf-8") as f:
            temp_file = f.read()
        with open("функции-принадлежности-управление.json", encoding="utf-8") as f:
            heat_filse = f.read()
        with open("функция-отображения.json", encoding="utf-8") as f:
            rules_file = f.read()

        print("Уровень нагрева (из файлов):",
              main(temp_file, heat_file, rules_file, 25))
    except FileNotFoundError:
        pass
