"""
Cardiovascular Disease Expert System
Backward chaining with certainty factors.

Educational AI project only. Not for real medical diagnosis.

Rule CF:
    CF(conclusion) = min(CF(premises)) * CF(rule_strength)

If more than one rule supports the same conclusion:
    CFcombined = CF1 + CF2 * (1 - CF1)
"""

from dataclasses import dataclass
from typing import Dict, List, Tuple, Set, Optional


@dataclass
class Rule:
    rule_id: str
    conclusion: str
    premises: List[str]
    strength: float
    priority: int


# -----------------------------
# RULE BASE
# -----------------------------
RULES: List[Rule] = [
    Rule("R01", "Acute Myocardial Infarction",
         ["pressure_like_chest_pain", "chest_pain_over_20_min", "elevated_troponin"], 0.98, 100),
    Rule("R02", "Acute Myocardial Infarction",
         ["acute_ischemic_ecg_changes", "pressure_like_chest_pain", "diaphoresis_or_nausea"], 0.98, 100),

    Rule("R03", "Aortic Dissection",
         ["sudden_severe_chest_or_back_pain", "tearing_or_ripping_pain", "interarm_bp_or_pulse_difference"], 0.96, 99),
    Rule("R04", "Aortic Dissection",
         ["ct_aortic_intimal_flap", "sudden_severe_chest_or_back_pain"], 0.99, 99),

    Rule("R05", "Infective Endocarditis",
         ["persistent_fever", "positive_blood_cultures", "new_or_changing_heart_murmur"], 0.97, 93),
    Rule("R06", "Infective Endocarditis",
         ["valve_vegetation_on_echo", "persistent_bacteremia"], 0.99, 94),

    Rule("R07", "Acute Pericarditis",
         ["sharp_or_pleuritic_chest_pain", "pain_worse_lying_flat", "pain_better_leaning_forward"], 0.91, 90),
    Rule("R08", "Acute Pericarditis",
         ["diffuse_st_elevation_or_pr_depression", "positional_chest_pain", "pericardial_friction_rub"], 0.96, 91),

    Rule("R09", "Atrial Fibrillation",
         ["irregularly_irregular_pulse", "absent_consistent_p_waves"], 0.98, 88),
    Rule("R10", "Atrial Fibrillation",
         ["palpitations", "irregular_ventricular_rhythm", "variable_rr_intervals"], 0.95, 87),

    Rule("R11", "Heart Failure",
         ["shortness_of_breath", "orthopnea_or_pnd", "bilateral_leg_edema"], 0.90, 85),
    Rule("R12", "Heart Failure",
         ["reduced_ventricular_function", "elevated_bnp", "pulmonary_congestion_or_crackles"], 0.96, 86),

    Rule("R13", "Supraventricular Tachycardia",
         ["sudden_palpitations", "regular_tachycardia", "narrow_complex_tachycardia"], 0.95, 84),
    Rule("R14", "Supraventricular Tachycardia",
         ["abrupt_tachycardia_onset", "abrupt_tachycardia_end", "narrow_qrs"], 0.92, 83),

    Rule("R15", "Symptomatic Bradycardia",
         ["heart_rate_below_50", "dizziness_weakness_or_syncope"], 0.91, 82),
    Rule("R16", "Symptomatic Bradycardia",
         ["slow_rhythm_on_ecg", "hypotension_or_marked_fatigue", "symptoms_during_slow_rate"], 0.93, 81),

    Rule("R17", "Aortic Stenosis",
         ["crescendo_decrescendo_systolic_murmur", "murmur_radiates_to_carotids", "exertional_dyspnea_chest_pain_or_syncope"], 0.96, 79),
    Rule("R18", "Aortic Stenosis",
         ["narrowed_aortic_valve_on_echo", "increased_aortic_valve_gradient"], 0.99, 80),

    Rule("R19", "Mitral Regurgitation",
         ["holosystolic_apical_murmur", "murmur_radiates_to_axilla", "dyspnea_or_fatigue"], 0.94, 77),
    Rule("R20", "Mitral Regurgitation",
         ["mitral_regurgitant_flow_on_echo", "left_atrial_or_ventricular_enlargement"], 0.98, 78),

    Rule("R21", "Hypertrophic Cardiomyopathy",
         ["lv_wall_thickening", "exertional_dyspnea_or_syncope", "family_history_hcm_or_sudden_death"], 0.96, 76),
    Rule("R22", "Hypertrophic Cardiomyopathy",
         ["murmur_louder_with_valsalva", "exertional_dizziness_syncope_or_dyspnea"], 0.92, 75),

    Rule("R23", "Dilated Cardiomyopathy",
         ["enlarged_left_ventricle", "reduced_ejection_fraction", "global_hypokinesis"], 0.97, 74),
    Rule("R24", "Dilated Cardiomyopathy",
         ["heart_failure_symptoms", "cardiomegaly", "no_major_primary_valve_disorder"], 0.87, 73),

    Rule("R25", "Peripheral Artery Disease",
         ["leg_pain_with_walking", "leg_pain_better_with_rest", "reduced_pedal_pulses"], 0.92, 70),
    Rule("R26", "Peripheral Artery Disease",
         ["low_ankle_brachial_index", "exertional_leg_discomfort"], 0.97, 71),

    Rule("R27", "Stable Angina",
         ["exertional_chest_pressure", "chest_pain_better_with_rest", "predictable_chest_pain_pattern"], 0.92, 66),
    Rule("R28", "Stable Angina",
         ["stress_test_ischemia", "troponin_not_elevated", "symptoms_with_exertion"], 0.93, 67),

    Rule("R29", "Hypertension",
         ["repeated_elevated_office_bp", "elevated_bp_on_separate_days"], 0.95, 60),
    Rule("R30", "Hypertension",
         ["elevated_home_or_ambulatory_bp", "multiple_out_of_office_measurements"], 0.97, 61),
]


# -----------------------------
# HYPOTHETICAL PATIENT FACTS
# -----------------------------
PATIENTS: Dict[int, Dict[str, float]] = {
    1: {
        "pressure_like_chest_pain": 0.95,
        "chest_pain_over_20_min": 0.92,
        "elevated_troponin": 0.99,
        "diaphoresis_or_nausea": 0.87,
        "acute_ischemic_ecg_changes": 0.96,
    },
    2: {
        "sudden_severe_chest_or_back_pain": 0.97,
        "tearing_or_ripping_pain": 0.93,
        "interarm_bp_or_pulse_difference": 0.88,
        "ct_aortic_intimal_flap": 0.99,
    },
    3: {
        "sharp_or_pleuritic_chest_pain": 0.91,
        "pain_worse_lying_flat": 0.94,
        "pain_better_leaning_forward": 0.95,
        "diffuse_st_elevation_or_pr_depression": 0.93,
        "positional_chest_pain": 0.95,
        "pericardial_friction_rub": 0.89,
    },
    4: {
        "shortness_of_breath": 0.94,
        "orthopnea_or_pnd": 0.91,
        "bilateral_leg_edema": 0.87,
        "elevated_bnp": 0.90,
        "reduced_ventricular_function": 0.95,
        "pulmonary_congestion_or_crackles": 0.86,
    },
    5: {
        "palpitations": 0.88,
        "irregularly_irregular_pulse": 0.97,
        "absent_consistent_p_waves": 0.99,
        "irregular_ventricular_rhythm": 0.98,
        "variable_rr_intervals": 0.96,
    },
    6: {
        "sudden_palpitations": 0.94,
        "regular_tachycardia": 0.96,
        "narrow_complex_tachycardia": 0.98,
        "abrupt_tachycardia_onset": 0.93,
        "abrupt_tachycardia_end": 0.93,
        "narrow_qrs": 0.98,
    },
    7: {
        "heart_rate_below_50": 0.99,
        "dizziness_weakness_or_syncope": 0.90,
        "slow_rhythm_on_ecg": 0.97,
        "hypotension_or_marked_fatigue": 0.84,
        "symptoms_during_slow_rate": 0.94,
    },
    8: {
        "crescendo_decrescendo_systolic_murmur": 0.96,
        "murmur_radiates_to_carotids": 0.94,
        "exertional_dyspnea_chest_pain_or_syncope": 0.86,
        "narrowed_aortic_valve_on_echo": 0.99,
        "increased_aortic_valve_gradient": 0.97,
    },
    9: {
        "holosystolic_apical_murmur": 0.96,
        "murmur_radiates_to_axilla": 0.94,
        "dyspnea_or_fatigue": 0.85,
        "mitral_regurgitant_flow_on_echo": 0.99,
        "left_atrial_or_ventricular_enlargement": 0.89,
    },
    10: {
        "persistent_fever": 0.94,
        "positive_blood_cultures": 0.98,
        "new_or_changing_heart_murmur": 0.91,
        "valve_vegetation_on_echo": 0.97,
        "persistent_bacteremia": 0.96,
    },
    11: {
        "repeated_elevated_office_bp": 0.96,
        "elevated_bp_on_separate_days": 0.95,
        "elevated_home_or_ambulatory_bp": 0.94,
        "multiple_out_of_office_measurements": 0.97,
    },
    12: {
        "leg_pain_with_walking": 0.93,
        "leg_pain_better_with_rest": 0.95,
        "reduced_pedal_pulses": 0.86,
        "low_ankle_brachial_index": 0.97,
        "exertional_leg_discomfort": 0.93,
    },
    13: {
        "enlarged_left_ventricle": 0.97,
        "reduced_ejection_fraction": 0.96,
        "global_hypokinesis": 0.92,
        "heart_failure_symptoms": 0.86,
        "cardiomegaly": 0.89,
        "no_major_primary_valve_disorder": 0.84,
    },
    14: {
        "lv_wall_thickening": 0.98,
        "exertional_dyspnea_or_syncope": 0.89,
        "family_history_hcm_or_sudden_death": 0.91,
        "murmur_louder_with_valsalva": 0.94,
        "exertional_dizziness_syncope_or_dyspnea": 0.89,
    },
    15: {
        "exertional_chest_pressure": 0.94,
        "chest_pain_better_with_rest": 0.95,
        "predictable_chest_pain_pattern": 0.90,
        "stress_test_ischemia": 0.92,
        "troponin_not_elevated": 0.93,
        "symptoms_with_exertion": 0.94,
    },
}


DIAGNOSES = sorted({rule.conclusion for rule in RULES})


def combine_cf(cf1: float, cf2: float) -> float:
    """Combine two positive certainty factors."""
    return cf1 + cf2 * (1.0 - cf1)


def prove_premise(
    premise: str,
    facts: Dict[str, float],
    visited: Set[str]
) -> Tuple[float, List[str]]:
    """
    Try to prove a premise.

    First checks direct patient facts.
    If the premise is not a direct fact, it can recursively attempt
    to prove it as a rule conclusion.
    """
    if premise in facts:
        return facts[premise], [f"FACT {premise} = {facts[premise]:.3f}"]

    if premise in visited:
        return 0.0, [f"CYCLE BLOCKED for {premise}"]

    matching_rules = [r for r in RULES if r.conclusion == premise]
    if not matching_rules:
        return 0.0, [f"MISSING FACT {premise}"]

    cf, trace = backward_chain(premise, facts, visited | {premise})
    return cf, trace


def backward_chain(
    goal: str,
    facts: Dict[str, float],
    visited: Optional[Set[str]] = None
) -> Tuple[float, List[str]]:
    """
    Backward-chain from a diagnosis goal to its supporting rules and premises.
    """
    if visited is None:
        visited = set()

    supporting_rules = sorted(
        [rule for rule in RULES if rule.conclusion == goal],
        key=lambda r: r.priority,
        reverse=True,
    )

    if not supporting_rules:
        return 0.0, [f"No rules support goal: {goal}"]

    combined_cf = 0.0
    trace: List[str] = [f"\nGOAL: {goal}"]

    for rule in supporting_rules:
        trace.append(
            f"\nChecking {rule.rule_id} "
            f"(priority={rule.priority}, strength={rule.strength:.2f})"
        )

        premise_cfs: List[float] = []
        rule_supported = True

        for premise in rule.premises:
            premise_cf, premise_trace = prove_premise(premise, facts, visited | {goal})
            trace.extend("  " + line for line in premise_trace)

            if premise_cf <= 0.0:
                rule_supported = False
                trace.append(f"  {rule.rule_id} cannot fire: {premise} is not supported.")
                break

            premise_cfs.append(premise_cf)

        if not rule_supported:
            continue

        min_premise_cf = min(premise_cfs)
        rule_cf = min_premise_cf * rule.strength

        trace.append(
            f"  min(premise CFs) = {min_premise_cf:.3f}"
        )
        trace.append(
            f"  {rule.rule_id} conclusion CF = "
            f"{min_premise_cf:.3f} x {rule.strength:.3f} = {rule_cf:.3f}"
        )

        previous_cf = combined_cf
        combined_cf = combine_cf(combined_cf, rule_cf)

        if previous_cf == 0.0:
            trace.append(f"  Combined CF = {combined_cf:.3f}")
        else:
            trace.append(
                f"  Combined CF = {previous_cf:.3f} + "
                f"{rule_cf:.3f}(1 - {previous_cf:.3f}) = {combined_cf:.3f}"
            )

    return combined_cf, trace


def diagnose_patient(patient_id: int, show_trace: bool = False) -> List[Tuple[str, float]]:
    facts = PATIENTS[patient_id]
    results = []

    for diagnosis in DIAGNOSES:
        cf, trace = backward_chain(diagnosis, facts)
        if cf > 0.0:
            results.append((diagnosis, cf))
            if show_trace:
                print("\n".join(trace))

    results.sort(key=lambda item: item[1], reverse=True)
    return results


def print_patient_facts(patient_id: int) -> None:
    print(f"\nPatient {patient_id} facts:")
    for fact, cf in PATIENTS[patient_id].items():
        print(f"  {fact:<45} CF={cf:.2f}")


def main() -> None:
    print("=" * 72)
    print("CARDIOVASCULAR DISEASE EXPERT SYSTEM")
    print("Backward Chaining with Certainty Factors")
    print("=" * 72)
    print("Educational project only - not for real medical diagnosis.\n")

    while True:
        print("Options:")
        print("1. Diagnose one hypothetical patient")
        print("2. Diagnose all 15 hypothetical patients")
        print("3. Test one diagnosis goal for one patient with full trace")
        print("0. Exit")

        choice = input("\nEnter choice: ").strip()

        if choice == "0":
            print("Exiting.")
            break

        elif choice == "1":
            try:
                patient_id = int(input("Enter patient number (1-15): "))
                if patient_id not in PATIENTS:
                    raise ValueError
            except ValueError:
                print("Invalid patient number.\n")
                continue

            print_patient_facts(patient_id)
            results = diagnose_patient(patient_id)

            print("\nSupported diagnoses:")
            if not results:
                print("  No diagnosis was fully supported by the available facts.")
            else:
                for diagnosis, cf in results:
                    print(f"  {diagnosis:<35} CF={cf:.3f}")

                print(
                    f"\nHighest-supported diagnosis: {results[0][0]} "
                    f"(CF={results[0][1]:.3f})"
                )
            print()

        elif choice == "2":
            for patient_id in range(1, 16):
                results = diagnose_patient(patient_id)
                if results:
                    best_diagnosis, best_cf = results[0]
                    print(
                        f"Patient {patient_id:2d}: "
                        f"{best_diagnosis:<35} CF={best_cf:.3f}"
                    )
                else:
                    print(f"Patient {patient_id:2d}: No supported diagnosis")
            print()

        elif choice == "3":
            try:
                patient_id = int(input("Enter patient number (1-15): "))
                if patient_id not in PATIENTS:
                    raise ValueError
            except ValueError:
                print("Invalid patient number.\n")
                continue

            print("\nAvailable diagnosis goals:")
            for index, diagnosis in enumerate(DIAGNOSES, start=1):
                print(f"{index:2d}. {diagnosis}")

            try:
                diagnosis_number = int(input("\nSelect diagnosis number: "))
                if not 1 <= diagnosis_number <= len(DIAGNOSES):
                    raise ValueError
            except ValueError:
                print("Invalid diagnosis number.\n")
                continue

            goal = DIAGNOSES[diagnosis_number - 1]
            print_patient_facts(patient_id)

            cf, trace = backward_chain(goal, PATIENTS[patient_id])
            print("\nBACKWARD-CHAINING TRACE")
            print("-" * 72)
            print("\n".join(trace))
            print("-" * 72)
            print(f"Final CF for {goal}: {cf:.3f}\n")

        else:
            print("Invalid choice.\n")


if __name__ == "__main__":
    main()
