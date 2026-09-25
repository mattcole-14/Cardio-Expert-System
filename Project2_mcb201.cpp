/*
Cardiovascular Disease Expert System
Backward chaining with certainty factors.

Educational AI project only. Not for real medical diagnosis.

Rule CF:
    CF(conclusion) = min(CF(premises)) * CF(rule strength)

If more than one rule supports the same conclusion:
    CFcombined = CF1 + CF2 * (1 - CF1)

Compile:
    g++ -std=c++17 cardiovascular_expert_system.cpp -o cardiovascular_expert_system

Run:
    ./cardiovascular_expert_system
*/

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Rule {
    string id;
    string conclusion;
    vector<string> premises;
    double strength;
    int priority;
};

using FactBase = map<string, double>;

vector<Rule> rules = {
    {"R01", "Acute Myocardial Infarction",
     {"pressure_like_chest_pain", "chest_pain_over_20_min", "elevated_troponin"}, 0.98, 100},
    {"R02", "Acute Myocardial Infarction",
     {"acute_ischemic_ecg_changes", "pressure_like_chest_pain", "diaphoresis_or_nausea"}, 0.98, 100},

    {"R03", "Aortic Dissection",
     {"sudden_severe_chest_or_back_pain", "tearing_or_ripping_pain", "interarm_bp_or_pulse_difference"}, 0.96, 99},
    {"R04", "Aortic Dissection",
     {"ct_aortic_intimal_flap", "sudden_severe_chest_or_back_pain"}, 0.99, 99},

    {"R05", "Infective Endocarditis",
     {"persistent_fever", "positive_blood_cultures", "new_or_changing_heart_murmur"}, 0.97, 93},
    {"R06", "Infective Endocarditis",
     {"valve_vegetation_on_echo", "persistent_bacteremia"}, 0.99, 94},

    {"R07", "Acute Pericarditis",
     {"sharp_or_pleuritic_chest_pain", "pain_worse_lying_flat", "pain_better_leaning_forward"}, 0.91, 90},
    {"R08", "Acute Pericarditis",
     {"diffuse_st_elevation_or_pr_depression", "positional_chest_pain", "pericardial_friction_rub"}, 0.96, 91},

    {"R09", "Atrial Fibrillation",
     {"irregularly_irregular_pulse", "absent_consistent_p_waves"}, 0.98, 88},
    {"R10", "Atrial Fibrillation",
     {"palpitations", "irregular_ventricular_rhythm", "variable_rr_intervals"}, 0.95, 87},

    {"R11", "Heart Failure",
     {"shortness_of_breath", "orthopnea_or_pnd", "bilateral_leg_edema"}, 0.90, 85},
    {"R12", "Heart Failure",
     {"reduced_ventricular_function", "elevated_bnp", "pulmonary_congestion_or_crackles"}, 0.96, 86},

    {"R13", "Supraventricular Tachycardia",
     {"sudden_palpitations", "regular_tachycardia", "narrow_complex_tachycardia"}, 0.95, 84},
    {"R14", "Supraventricular Tachycardia",
     {"abrupt_tachycardia_onset", "abrupt_tachycardia_end", "narrow_qrs"}, 0.92, 83},

    {"R15", "Symptomatic Bradycardia",
     {"heart_rate_below_50", "dizziness_weakness_or_syncope"}, 0.91, 82},
    {"R16", "Symptomatic Bradycardia",
     {"slow_rhythm_on_ecg", "hypotension_or_marked_fatigue", "symptoms_during_slow_rate"}, 0.93, 81},

    {"R17", "Aortic Stenosis",
     {"crescendo_decrescendo_systolic_murmur", "murmur_radiates_to_carotids", "exertional_dyspnea_chest_pain_or_syncope"}, 0.96, 79},
    {"R18", "Aortic Stenosis",
     {"narrowed_aortic_valve_on_echo", "increased_aortic_valve_gradient"}, 0.99, 80},

    {"R19", "Mitral Regurgitation",
     {"holosystolic_apical_murmur", "murmur_radiates_to_axilla", "dyspnea_or_fatigue"}, 0.94, 77},
    {"R20", "Mitral Regurgitation",
     {"mitral_regurgitant_flow_on_echo", "left_atrial_or_ventricular_enlargement"}, 0.98, 78},

    {"R21", "Hypertrophic Cardiomyopathy",
     {"lv_wall_thickening", "exertional_dyspnea_or_syncope", "family_history_hcm_or_sudden_death"}, 0.96, 76},
    {"R22", "Hypertrophic Cardiomyopathy",
     {"murmur_louder_with_valsalva", "exertional_dizziness_syncope_or_dyspnea"}, 0.92, 75},

    {"R23", "Dilated Cardiomyopathy",
     {"enlarged_left_ventricle", "reduced_ejection_fraction", "global_hypokinesis"}, 0.97, 74},
    {"R24", "Dilated Cardiomyopathy",
     {"heart_failure_symptoms", "cardiomegaly", "no_major_primary_valve_disorder"}, 0.87, 73},

    {"R25", "Peripheral Artery Disease",
     {"leg_pain_with_walking", "leg_pain_better_with_rest", "reduced_pedal_pulses"}, 0.92, 70},
    {"R26", "Peripheral Artery Disease",
     {"low_ankle_brachial_index", "exertional_leg_discomfort"}, 0.97, 71},

    {"R27", "Stable Angina",
     {"exertional_chest_pressure", "chest_pain_better_with_rest", "predictable_chest_pain_pattern"}, 0.92, 66},
    {"R28", "Stable Angina",
     {"stress_test_ischemia", "troponin_not_elevated", "symptoms_with_exertion"}, 0.93, 67},

    {"R29", "Hypertension",
     {"repeated_elevated_office_bp", "elevated_bp_on_separate_days"}, 0.95, 60},
    {"R30", "Hypertension",
     {"elevated_home_or_ambulatory_bp", "multiple_out_of_office_measurements"}, 0.97, 61},
};

map<int, FactBase> patients = {
    {1, {
        {"pressure_like_chest_pain", 0.95},
        {"chest_pain_over_20_min", 0.92},
        {"elevated_troponin", 0.99},
        {"diaphoresis_or_nausea", 0.87},
        {"acute_ischemic_ecg_changes", 0.96}
    }},
    {2, {
        {"sudden_severe_chest_or_back_pain", 0.97},
        {"tearing_or_ripping_pain", 0.93},
        {"interarm_bp_or_pulse_difference", 0.88},
        {"ct_aortic_intimal_flap", 0.99}
    }},
    {3, {
        {"sharp_or_pleuritic_chest_pain", 0.91},
        {"pain_worse_lying_flat", 0.94},
        {"pain_better_leaning_forward", 0.95},
        {"diffuse_st_elevation_or_pr_depression", 0.93},
        {"positional_chest_pain", 0.95},
        {"pericardial_friction_rub", 0.89}
    }},
    {4, {
        {"shortness_of_breath", 0.94},
        {"orthopnea_or_pnd", 0.91},
        {"bilateral_leg_edema", 0.87},
        {"elevated_bnp", 0.90},
        {"reduced_ventricular_function", 0.95},
        {"pulmonary_congestion_or_crackles", 0.86}
    }},
    {5, {
        {"palpitations", 0.88},
        {"irregularly_irregular_pulse", 0.97},
        {"absent_consistent_p_waves", 0.99},
        {"irregular_ventricular_rhythm", 0.98},
        {"variable_rr_intervals", 0.96}
    }},
    {6, {
        {"sudden_palpitations", 0.94},
        {"regular_tachycardia", 0.96},
        {"narrow_complex_tachycardia", 0.98},
        {"abrupt_tachycardia_onset", 0.93},
        {"abrupt_tachycardia_end", 0.93},
        {"narrow_qrs", 0.98}
    }},
    {7, {
        {"heart_rate_below_50", 0.99},
        {"dizziness_weakness_or_syncope", 0.90},
        {"slow_rhythm_on_ecg", 0.97},
        {"hypotension_or_marked_fatigue", 0.84},
        {"symptoms_during_slow_rate", 0.94}
    }},
    {8, {
        {"crescendo_decrescendo_systolic_murmur", 0.96},
        {"murmur_radiates_to_carotids", 0.94},
        {"exertional_dyspnea_chest_pain_or_syncope", 0.86},
        {"narrowed_aortic_valve_on_echo", 0.99},
        {"increased_aortic_valve_gradient", 0.97}
    }},
    {9, {
        {"holosystolic_apical_murmur", 0.96},
        {"murmur_radiates_to_axilla", 0.94},
        {"dyspnea_or_fatigue", 0.85},
        {"mitral_regurgitant_flow_on_echo", 0.99},
        {"left_atrial_or_ventricular_enlargement", 0.89}
    }},
    {10, {
        {"persistent_fever", 0.94},
        {"positive_blood_cultures", 0.98},
        {"new_or_changing_heart_murmur", 0.91},
        {"valve_vegetation_on_echo", 0.97},
        {"persistent_bacteremia", 0.96}
    }},
    {11, {
        {"repeated_elevated_office_bp", 0.96},
        {"elevated_bp_on_separate_days", 0.95},
        {"elevated_home_or_ambulatory_bp", 0.94},
        {"multiple_out_of_office_measurements", 0.97}
    }},
    {12, {
        {"leg_pain_with_walking", 0.93},
        {"leg_pain_better_with_rest", 0.95},
        {"reduced_pedal_pulses", 0.86},
        {"low_ankle_brachial_index", 0.97},
        {"exertional_leg_discomfort", 0.93}
    }},
    {13, {
        {"enlarged_left_ventricle", 0.97},
        {"reduced_ejection_fraction", 0.96},
        {"global_hypokinesis", 0.92},
        {"heart_failure_symptoms", 0.86},
        {"cardiomegaly", 0.89},
        {"no_major_primary_valve_disorder", 0.84}
    }},
    {14, {
        {"lv_wall_thickening", 0.98},
        {"exertional_dyspnea_or_syncope", 0.89},
        {"family_history_hcm_or_sudden_death", 0.91},
        {"murmur_louder_with_valsalva", 0.94},
        {"exertional_dizziness_syncope_or_dyspnea", 0.89}
    }},
    {15, {
        {"exertional_chest_pressure", 0.94},
        {"chest_pain_better_with_rest", 0.95},
        {"predictable_chest_pain_pattern", 0.90},
        {"stress_test_ischemia", 0.92},
        {"troponin_not_elevated", 0.93},
        {"symptoms_with_exertion", 0.94}
    }}
};

double combineCF(double cf1, double cf2) {
    return cf1 + cf2 * (1.0 - cf1);
}

pair<double, vector<string>> backwardChain(
    const string& goal,
    const FactBase& facts,
    set<string> visited
);

pair<double, vector<string>> provePremise(
    const string& premise,
    const FactBase& facts,
    set<string> visited
) {
    auto factIt = facts.find(premise);
    if (factIt != facts.end()) {
        vector<string> trace;
        trace.push_back("FACT " + premise + " = " + to_string(factIt->second));
        return {factIt->second, trace};
    }

    if (visited.count(premise)) {
        return {0.0, {"CYCLE BLOCKED for " + premise}};
    }

    bool hasRule = false;
    for (const auto& rule : rules) {
        if (rule.conclusion == premise) {
            hasRule = true;
            break;
        }
    }

    if (!hasRule) {
        return {0.0, {"MISSING FACT " + premise}};
    }

    visited.insert(premise);
    return backwardChain(premise, facts, visited);
}

pair<double, vector<string>> backwardChain(
    const string& goal,
    const FactBase& facts,
    set<string> visited
) {
    vector<Rule> supportingRules;

    for (const auto& rule : rules) {
        if (rule.conclusion == goal) {
            supportingRules.push_back(rule);
        }
    }

    sort(supportingRules.begin(), supportingRules.end(),
         [](const Rule& a, const Rule& b) {
             return a.priority > b.priority;
         });

    if (supportingRules.empty()) {
        return {0.0, {"No rules support goal: " + goal}};
    }

    double combinedCF = 0.0;
    vector<string> trace;
    trace.push_back("\nGOAL: " + goal);

    for (const auto& rule : supportingRules) {
        trace.push_back(
            "\nChecking " + rule.id +
            " (priority=" + to_string(rule.priority) +
            ", strength=" + to_string(rule.strength) + ")"
        );

        vector<double> premiseCFs;
        bool supported = true;

        for (const auto& premise : rule.premises) {
            set<string> nextVisited = visited;
            nextVisited.insert(goal);

            auto result = provePremise(premise, facts, nextVisited);
            double premiseCF = result.first;

            for (const auto& line : result.second) {
                trace.push_back("  " + line);
            }

            if (premiseCF <= 0.0) {
                supported = false;
                trace.push_back(
                    "  " + rule.id + " cannot fire: " +
                    premise + " is not supported."
                );
                break;
            }

            premiseCFs.push_back(premiseCF);
        }

        if (!supported) {
            continue;
        }

        double minPremiseCF = *min_element(
            premiseCFs.begin(), premiseCFs.end()
        );

        double ruleCF = minPremiseCF * rule.strength;

        trace.push_back(
            "  min(premise CFs) = " + to_string(minPremiseCF)
        );

        trace.push_back(
            "  " + rule.id + " conclusion CF = " +
            to_string(minPremiseCF) + " x " +
            to_string(rule.strength) + " = " +
            to_string(ruleCF)
        );

        double previousCF = combinedCF;
        combinedCF = combineCF(combinedCF, ruleCF);

        if (previousCF == 0.0) {
            trace.push_back(
                "  Combined CF = " + to_string(combinedCF)
            );
        } else {
            trace.push_back(
                "  Combined CF = " + to_string(previousCF) +
                " + " + to_string(ruleCF) +
                "(1 - " + to_string(previousCF) +
                ") = " + to_string(combinedCF)
            );
        }
    }

    return {combinedCF, trace};
}

vector<string> getDiagnoses() {
    set<string> unique;
    for (const auto& rule : rules) {
        unique.insert(rule.conclusion);
    }
    return vector<string>(unique.begin(), unique.end());
}

vector<pair<string, double>> diagnosePatient(int patientId) {
    const FactBase& facts = patients.at(patientId);
    vector<pair<string, double>> results;

    for (const auto& diagnosis : getDiagnoses()) {
        auto result = backwardChain(diagnosis, facts, {});
        if (result.first > 0.0) {
            results.push_back({diagnosis, result.first});
        }
    }

    sort(results.begin(), results.end(),
         [](const auto& a, const auto& b) {
             return a.second > b.second;
         });

    return results;
}

void printPatientFacts(int patientId) {
    cout << "\nPatient " << patientId << " facts:\n";

    for (const auto& [fact, cf] : patients.at(patientId)) {
        cout << "  " << left << setw(45) << fact
             << " CF=" << fixed << setprecision(2) << cf << '\n';
    }
}

int main() {
    cout << string(72, '=') << '\n';
    cout << "CARDIOVASCULAR DISEASE EXPERT SYSTEM\n";
    cout << "Backward Chaining with Certainty Factors\n";
    cout << string(72, '=') << '\n';
    cout << "Educational project only - not for real medical diagnosis.\n\n";

    while (true) {
        cout << "Options:\n";
        cout << "1. Diagnose one hypothetical patient\n";
        cout << "2. Diagnose all 15 hypothetical patients\n";
        cout << "3. Test one diagnosis goal for one patient with full trace\n";
        cout << "0. Exit\n";
        cout << "\nEnter choice: ";

        int choice;
        cin >> choice;

        if (!cin) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n\n";
            continue;
        }

        if (choice == 0) {
            cout << "Exiting.\n";
            break;
        }

        if (choice == 1) {
            int patientId;
            cout << "Enter patient number (1-15): ";
            cin >> patientId;

            if (!patients.count(patientId)) {
                cout << "Invalid patient number.\n\n";
                continue;
            }

            printPatientFacts(patientId);

            auto results = diagnosePatient(patientId);
            cout << "\nSupported diagnoses:\n";

            if (results.empty()) {
                cout << "  No diagnosis was fully supported by the available facts.\n";
            } else {
                for (const auto& [diagnosis, cf] : results) {
                    cout << "  " << left << setw(35) << diagnosis
                         << " CF=" << fixed << setprecision(3) << cf << '\n';
                }

                cout << "\nHighest-supported diagnosis: "
                     << results.front().first
                     << " (CF=" << fixed << setprecision(3)
                     << results.front().second << ")\n";
            }

            cout << '\n';
        }
        else if (choice == 2) {
            for (int patientId = 1; patientId <= 15; ++patientId) {
                auto results = diagnosePatient(patientId);

                if (!results.empty()) {
                    cout << "Patient " << setw(2) << patientId << ": "
                         << left << setw(35) << results.front().first
                         << " CF=" << fixed << setprecision(3)
                         << results.front().second << '\n';
                } else {
                    cout << "Patient " << patientId
                         << ": No supported diagnosis\n";
                }
            }
            cout << '\n';
        }
        else if (choice == 3) {
            int patientId;
            cout << "Enter patient number (1-15): ";
            cin >> patientId;

            if (!patients.count(patientId)) {
                cout << "Invalid patient number.\n\n";
                continue;
            }

            vector<string> diagnoses = getDiagnoses();

            cout << "\nAvailable diagnosis goals:\n";
            for (size_t i = 0; i < diagnoses.size(); ++i) {
                cout << setw(2) << (i + 1)
                     << ". " << diagnoses[i] << '\n';
            }

            int diagnosisNumber;
            cout << "\nSelect diagnosis number: ";
            cin >> diagnosisNumber;

            if (diagnosisNumber < 1 ||
                diagnosisNumber > static_cast<int>(diagnoses.size())) {
                cout << "Invalid diagnosis number.\n\n";
                continue;
            }

            string goal = diagnoses[diagnosisNumber - 1];
            printPatientFacts(patientId);

            auto result = backwardChain(goal, patients.at(patientId), {});

            cout << "\nBACKWARD-CHAINING TRACE\n";
            cout << string(72, '-') << '\n';

            for (const auto& line : result.second) {
                cout << line << '\n';
            }

            cout << string(72, '-') << '\n';
            cout << "Final CF for " << goal
                 << ": " << fixed << setprecision(3)
                 << result.first << "\n\n";
        }
        else {
            cout << "Invalid choice.\n\n";
        }
    }

    return 0;
}
