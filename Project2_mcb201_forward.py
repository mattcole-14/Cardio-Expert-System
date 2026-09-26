"""
Forward-Chaining Cardiovascular Treatment Expert System
Educational AI project only; not for real medical treatment decisions.

Finalized rule base: T01-T18. T01-T15 are unchanged; T16-T18 are additions.

Rule_CF(Conclusion) = min(CF(premises)) * CF(rule strength)
CF_combined = CF1 + CF2 * (1 - CF1)
"""

from dataclasses import dataclass
from typing import Dict, List, Tuple


@dataclass(frozen=True)
class TreatmentConclusion:
    name: str
    strength: float


@dataclass(frozen=True)
class TreatmentRule:
    rule_id: str
    diagnosis: str
    treatments: Tuple[TreatmentConclusion, ...]
    priority: int
    insertion_order: int


BACKWARD_CHAINING_FACTS: List[Tuple[str, float]] = [
    ("Acute Myocardial Infarction", 0.985),
    ("Aortic Dissection", 0.994),
    ("Acute Pericarditis", 0.975),
    ("Heart Failure", 0.962),
    ("Atrial Fibrillation", 0.992),
    ("Supraventricular Tachycardia", 0.985),
    ("Symptomatic Bradycardia", 0.960),
    ("Aortic Stenosis", 0.993),
    ("Mitral Regurgitation", 0.974),
    ("Infective Endocarditis", 0.994),
    ("Hypertension", 0.991),
    ("Peripheral Artery Disease", 0.980),
    ("Dilated Cardiomyopathy", 0.971),
    ("Hypertrophic Cardiomyopathy", 0.974),
    ("Stable Angina", 0.975),
]

RULES: List[TreatmentRule] = [
    TreatmentRule("T01", "Acute Myocardial Infarction", (
        TreatmentConclusion("Aspirin / antiplatelet therapy", 0.95),
        TreatmentConclusion("Percutaneous coronary intervention (PCI) / reperfusion therapy", 0.98),
        TreatmentConclusion("Anticoagulation", 0.88),
    ), 100, 0),

    TreatmentRule("T02", "Aortic Dissection", (
        TreatmentConclusion("Intravenous beta-blocker therapy for heart-rate and blood-pressure control", 0.93),
        TreatmentConclusion("Emergency surgical evaluation/repair when indicated", 0.98),
        TreatmentConclusion("Intensive blood-pressure management", 0.92),
    ), 99, 1),

    TreatmentRule("T03", "Infective Endocarditis", (
        TreatmentConclusion("Targeted intravenous antimicrobial therapy", 0.96),
        TreatmentConclusion("Valve surgery when severe valve damage or complications are present", 0.88),
    ), 94, 2),

    TreatmentRule("T04", "Acute Pericarditis", (
        TreatmentConclusion("Anti-inflammatory medication such as an NSAID", 0.92),
        TreatmentConclusion("Colchicine", 0.90),
        TreatmentConclusion("Activity restriction during active inflammation", 0.82),
    ), 91, 3),

    TreatmentRule("T05", "Atrial Fibrillation", (
        TreatmentConclusion("Rate-control therapy", 0.90),
        TreatmentConclusion("Rhythm-control therapy or cardioversion when appropriate", 0.84),
        TreatmentConclusion("Anticoagulation when stroke-risk assessment indicates it", 0.92),
    ), 88, 4),

    TreatmentRule("T06", "Heart Failure", (
        TreatmentConclusion("Guideline-directed heart-failure medication therapy", 0.94),
        TreatmentConclusion("Diuretic therapy for fluid congestion", 0.91),
        TreatmentConclusion("Sodium/fluid-management strategies when appropriate", 0.80),
    ), 86, 5),

    TreatmentRule("T07", "Supraventricular Tachycardia", (
        TreatmentConclusion("Vagal maneuvers when clinically appropriate", 0.82),
        TreatmentConclusion("Adenosine for appropriate regular narrow-complex SVT", 0.93),
        TreatmentConclusion("Synchronized cardioversion for unstable tachycardia", 0.96),
    ), 84, 6),

    TreatmentRule("T08", "Symptomatic Bradycardia", (
        TreatmentConclusion("Atropine when clinically appropriate", 0.90),
        TreatmentConclusion("Temporary cardiac pacing", 0.94),
        TreatmentConclusion("Treatment of the underlying cause", 0.88),
    ), 82, 7),

    TreatmentRule("T09", "Aortic Stenosis", (
        TreatmentConclusion("Aortic valve replacement when severe and indicated", 0.96),
        TreatmentConclusion("Transcatheter aortic valve replacement (TAVR) when appropriate", 0.92),
        TreatmentConclusion("Clinical and echocardiographic monitoring when intervention is not yet indicated", 0.80),
    ), 80, 8),

    TreatmentRule("T10", "Mitral Regurgitation", (
        TreatmentConclusion("Mitral valve repair when indicated", 0.94),
        TreatmentConclusion("Mitral valve replacement when repair is unsuitable", 0.88),
        TreatmentConclusion("Medical management of congestion and associated heart failure", 0.82),
    ), 78, 9),

    TreatmentRule("T11", "Hypertrophic Cardiomyopathy", (
        TreatmentConclusion("Beta-blocker therapy", 0.90),
        TreatmentConclusion("Appropriate nondihydropyridine calcium-channel blocker therapy", 0.82),
        TreatmentConclusion("Septal reduction therapy for selected severe obstructive disease", 0.85),
    ), 76, 10),

    TreatmentRule("T12", "Dilated Cardiomyopathy", (
        TreatmentConclusion("Guideline-directed heart-failure therapy", 0.94),
        TreatmentConclusion("Diuretic therapy for fluid overload", 0.88),
        TreatmentConclusion("Cardiac device therapy in selected qualifying patients", 0.80),
    ), 74, 11),

    TreatmentRule("T13", "Peripheral Artery Disease", (
        TreatmentConclusion("Antiplatelet therapy when indicated", 0.88),
        TreatmentConclusion("Statin therapy", 0.91),
        TreatmentConclusion("Structured exercise therapy", 0.89),
        TreatmentConclusion("Revascularization for selected severe disease", 0.82),
    ), 71, 12),

    TreatmentRule("T14", "Stable Angina", (
        TreatmentConclusion("Antianginal therapy such as beta blockers", 0.88),
        TreatmentConclusion("Nitrate therapy for angina symptoms", 0.85),
        TreatmentConclusion("Preventive cardiovascular therapy such as statin/antiplatelet therapy when indicated", 0.91),
        TreatmentConclusion("Coronary revascularization in selected patients", 0.78),
    ), 67, 13),

    TreatmentRule("T15", "Hypertension", (
        TreatmentConclusion("Lifestyle modification", 0.88),
        TreatmentConclusion("ACE inhibitor or ARB therapy when appropriate", 0.88),
        TreatmentConclusion("Thiazide-type diuretic therapy", 0.86),
        TreatmentConclusion("Calcium-channel blocker therapy", 0.86),
    ), 61, 14),

    # T16 deliberately overlaps T06. Its lower priority (84 vs. 86) means
    # T06 must execute first even though T16 has a higher computed rule CF.
    # The repeated diuretic conclusion also forces CF_combined to execute.
    TreatmentRule("T16", "Heart Failure", (
        TreatmentConclusion("Diuretic therapy for fluid congestion", 0.99),
    ), 84, 15),

    # T17 deliberately ties T05 on priority 88. T05 has the higher computed
    # rule CF, so the second tie-break criterion determines execution order.
    # Rate-control therapy is supported by both T05 and T17.
    TreatmentRule("T17", "Atrial Fibrillation", (
        TreatmentConclusion("Rate-control therapy", 0.86),
    ), 88, 16),

    # T18 adds another overlapping Stable Angina rule and preserves the
    # existing treatment terminology while giving an additional CF combination.
    TreatmentRule("T18", "Stable Angina", (
        TreatmentConclusion("Nitrate therapy for angina symptoms", 0.80),
        TreatmentConclusion("Antianginal therapy such as beta blockers", 0.83),
    ), 65, 17),
]


def combine_positive_cf(cf1: float, cf2: float) -> float:
    return cf1 + cf2 * (1.0 - cf1)


def validate_cf(cf: float) -> bool:
    return 0.0 <= cf <= 1.0


def treatment_rule_cf(diagnosis_cf: float, strength: float) -> float:
    minimum_premise_cf = diagnosis_cf
    return minimum_premise_cf * strength


def highest_computed_rule_cf(rule: TreatmentRule, diagnosis_cf: float) -> float:
    return max(
        treatment_rule_cf(diagnosis_cf, conclusion.strength)
        for conclusion in rule.treatments
    )


def applicable_rules(diagnosis: str, diagnosis_cf: float) -> List[TreatmentRule]:
    matches = [r for r in RULES if r.diagnosis == diagnosis]
    return sorted(
        matches,
        key=lambda r: (
            -r.priority,
            -highest_computed_rule_cf(r, diagnosis_cf),
            r.insertion_order,
            r.rule_id,
        ),
    )


def run_forward_chaining(
    diagnosis: str,
    diagnosis_cf: float,
    show_trace: bool = True,
) -> List[Tuple[str, float]]:
    if not validate_cf(diagnosis_cf):
        raise ValueError("Diagnosis certainty factor must be between 0 and 1.")

    rules_to_fire = applicable_rules(diagnosis, diagnosis_cf)

    if show_trace:
        print("=" * 80)
        print("FORWARD-CHAINING TREATMENT EXECUTION TRACE")
        print("=" * 80)
        print(f"Input diagnosis : {diagnosis}")
        print(f"Diagnosis CF    : {diagnosis_cf:.3f}")
        print()

    if not rules_to_fire:
        if show_trace:
            print("Applicable treatment rules: none")
            print("No treatment recommendations were produced.")
        return []

    if show_trace:
        print("Applicable treatment rules in execution order:")
        for i, rule in enumerate(rules_to_fire, 1):
            print(
                f"  {i}. {rule.rule_id} | priority={rule.priority} | "
                f"highest computed rule CF="
                f"{highest_computed_rule_cf(rule, diagnosis_cf):.3f} | "
                f"insertion order={rule.insertion_order}"
            )
        print()

    treatment_cfs: Dict[str, float] = {}
    treatment_first_seen: Dict[str, int] = {}
    seen_counter = 0

    for rule in rules_to_fire:
        if show_trace:
            print("-" * 80)
            print(f"FIRING RULE {rule.rule_id}")
            print(f"Diagnosis premise : {rule.diagnosis}")
            print(f"Rule priority     : {rule.priority}")
            print(f"Insertion order   : {rule.insertion_order}")
            print(f"Premise CFs       : [{diagnosis_cf:.3f}]")
            print(f"Minimum premise CF: {diagnosis_cf:.3f}")
            print()

        for conclusion in rule.treatments:
            min_premise_cf = diagnosis_cf
            rule_cf = min_premise_cf * conclusion.strength

            if show_trace:
                print(f"Treatment conclusion: {conclusion.name}")
                print(f"  Rule strength for this conclusion: {conclusion.strength:.3f}")
                print(
                    f"  Rule CF = min({diagnosis_cf:.3f}) x "
                    f"{conclusion.strength:.3f} = {rule_cf:.3f}"
                )

            if conclusion.name not in treatment_cfs:
                treatment_cfs[conclusion.name] = rule_cf
                treatment_first_seen[conclusion.name] = seen_counter
                seen_counter += 1
                if show_trace:
                    print(f"  First support -> combined CF = {rule_cf:.3f}")
            else:
                old_cf = treatment_cfs[conclusion.name]
                new_cf = combine_positive_cf(old_cf, rule_cf)
                treatment_cfs[conclusion.name] = new_cf
                if show_trace:
                    print(
                        f"  Additional support -> combined CF = "
                        f"{old_cf:.3f} + {rule_cf:.3f} x "
                        f"(1 - {old_cf:.3f}) = {new_cf:.3f}"
                    )
            if show_trace:
                print()

    final_results = sorted(
        treatment_cfs.items(),
        key=lambda item: (
            -item[1],
            treatment_first_seen[item[0]],
            item[0],
        ),
    )

    if show_trace:
        print("=" * 80)
        print("FINAL TREATMENT RECOMMENDATIONS")
        print("=" * 80)
        for i, (treatment, cf) in enumerate(final_results, 1):
            print(f"{i}. {treatment} | CF={cf:.3f}")
        print()

    return final_results


def print_diagnosis_list() -> None:
    print("\nStored backward-chaining diagnosis facts:")
    for i, (diagnosis, cf) in enumerate(BACKWARD_CHAINING_FACTS, 1):
        print(f"{i:2d}. {diagnosis:<35} CF={cf:.3f}")


def main() -> None:
    print("=" * 80)
    print("CARDIOVASCULAR FORWARD-CHAINING TREATMENT SYSTEM")
    print("=" * 80)
    print("Educational expert-system demonstration only.\n")

    while True:
        print("Options:")
        print("1. Use a stored backward-chaining diagnosis fact")
        print("2. Enter a diagnosis and diagnosis CF manually")
        print("3. Run all 15 stored diagnosis facts")
        print("0. Exit")

        choice = input("\nEnter choice: ").strip()

        if choice == "0":
            print("Exiting.")
            break

        if choice == "1":
            print_diagnosis_list()
            try:
                selection = int(input("\nSelect diagnosis number (1-15): ").strip())
                if not 1 <= selection <= len(BACKWARD_CHAINING_FACTS):
                    raise ValueError
            except ValueError:
                print("Invalid selection.\n")
                continue

            diagnosis, cf = BACKWARD_CHAINING_FACTS[selection - 1]
            run_forward_chaining(diagnosis, cf, True)

        elif choice == "2":
            print_diagnosis_list()
            diagnosis = input("\nEnter diagnosis exactly as listed: ").strip()
            try:
                cf = float(input("Enter diagnosis CF (0-1): ").strip())
            except ValueError:
                print("Invalid certainty factor.\n")
                continue

            if not validate_cf(cf):
                print("Diagnosis CF must be between 0 and 1.\n")
                continue

            run_forward_chaining(diagnosis, cf, True)

        elif choice == "3":
            for diagnosis, cf in BACKWARD_CHAINING_FACTS:
                run_forward_chaining(diagnosis, cf, True)

        else:
            print("Invalid choice.\n")


if __name__ == "__main__":
    main()
