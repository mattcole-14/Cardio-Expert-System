/*
Forward-Chaining Cardiovascular Treatment Expert System
Educational AI project only; not for real medical treatment decisions.

Finalized rule base: T01-T18. T01-T15 are unchanged; T16-T18 are additions.

Rule_CF(Conclusion) = min(CF(premises)) * CF(rule strength)
CF_combined = CF1 + CF2 * (1 - CF1)

Compile:
    g++ -std=c++17 cardiovascular_treatment_forward.cpp -o cardiovascular_treatment_forward

Run:
    ./cardiovascular_treatment_forward
*/

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct TreatmentConclusion {
    string name;
    double strength;
};

struct TreatmentRule {
    string ruleId;
    string diagnosis;
    vector<TreatmentConclusion> treatments;
    int priority;
    int insertionOrder;
};

const vector<pair<string, double>> BACKWARD_CHAINING_FACTS = {
    {"Acute Myocardial Infarction", 0.985},
    {"Aortic Dissection", 0.994},
    {"Acute Pericarditis", 0.975},
    {"Heart Failure", 0.962},
    {"Atrial Fibrillation", 0.992},
    {"Supraventricular Tachycardia", 0.985},
    {"Symptomatic Bradycardia", 0.960},
    {"Aortic Stenosis", 0.993},
    {"Mitral Regurgitation", 0.974},
    {"Infective Endocarditis", 0.994},
    {"Hypertension", 0.991},
    {"Peripheral Artery Disease", 0.980},
    {"Dilated Cardiomyopathy", 0.971},
    {"Hypertrophic Cardiomyopathy", 0.974},
    {"Stable Angina", 0.975},
};

const vector<TreatmentRule> RULES = {
    {"T01", "Acute Myocardial Infarction", {
        {"Aspirin / antiplatelet therapy", 0.95},
        {"Percutaneous coronary intervention (PCI) / reperfusion therapy", 0.98},
        {"Anticoagulation", 0.88},
    }, 100, 0},

    {"T02", "Aortic Dissection", {
        {"Intravenous beta-blocker therapy for heart-rate and blood-pressure control", 0.93},
        {"Emergency surgical evaluation/repair when indicated", 0.98},
        {"Intensive blood-pressure management", 0.92},
    }, 99, 1},

    {"T03", "Infective Endocarditis", {
        {"Targeted intravenous antimicrobial therapy", 0.96},
        {"Valve surgery when severe valve damage or complications are present", 0.88},
    }, 94, 2},

    {"T04", "Acute Pericarditis", {
        {"Anti-inflammatory medication such as an NSAID", 0.92},
        {"Colchicine", 0.90},
        {"Activity restriction during active inflammation", 0.82},
    }, 91, 3},

    {"T05", "Atrial Fibrillation", {
        {"Rate-control therapy", 0.90},
        {"Rhythm-control therapy or cardioversion when appropriate", 0.84},
        {"Anticoagulation when stroke-risk assessment indicates it", 0.92},
    }, 88, 4},

    {"T06", "Heart Failure", {
        {"Guideline-directed heart-failure medication therapy", 0.94},
        {"Diuretic therapy for fluid congestion", 0.91},
        {"Sodium/fluid-management strategies when appropriate", 0.80},
    }, 86, 5},

    {"T07", "Supraventricular Tachycardia", {
        {"Vagal maneuvers when clinically appropriate", 0.82},
        {"Adenosine for appropriate regular narrow-complex SVT", 0.93},
        {"Synchronized cardioversion for unstable tachycardia", 0.96},
    }, 84, 6},

    {"T08", "Symptomatic Bradycardia", {
        {"Atropine when clinically appropriate", 0.90},
        {"Temporary cardiac pacing", 0.94},
        {"Treatment of the underlying cause", 0.88},
    }, 82, 7},

    {"T09", "Aortic Stenosis", {
        {"Aortic valve replacement when severe and indicated", 0.96},
        {"Transcatheter aortic valve replacement (TAVR) when appropriate", 0.92},
        {"Clinical and echocardiographic monitoring when intervention is not yet indicated", 0.80},
    }, 80, 8},

    {"T10", "Mitral Regurgitation", {
        {"Mitral valve repair when indicated", 0.94},
        {"Mitral valve replacement when repair is unsuitable", 0.88},
        {"Medical management of congestion and associated heart failure", 0.82},
    }, 78, 9},

    {"T11", "Hypertrophic Cardiomyopathy", {
        {"Beta-blocker therapy", 0.90},
        {"Appropriate nondihydropyridine calcium-channel blocker therapy", 0.82},
        {"Septal reduction therapy for selected severe obstructive disease", 0.85},
    }, 76, 10},

    {"T12", "Dilated Cardiomyopathy", {
        {"Guideline-directed heart-failure therapy", 0.94},
        {"Diuretic therapy for fluid overload", 0.88},
        {"Cardiac device therapy in selected qualifying patients", 0.80},
    }, 74, 11},

    {"T13", "Peripheral Artery Disease", {
        {"Antiplatelet therapy when indicated", 0.88},
        {"Statin therapy", 0.91},
        {"Structured exercise therapy", 0.89},
        {"Revascularization for selected severe disease", 0.82},
    }, 71, 12},

    {"T14", "Stable Angina", {
        {"Antianginal therapy such as beta blockers", 0.88},
        {"Nitrate therapy for angina symptoms", 0.85},
        {"Preventive cardiovascular therapy such as statin/antiplatelet therapy when indicated", 0.91},
        {"Coronary revascularization in selected patients", 0.78},
    }, 67, 13},

    {"T15", "Hypertension", {
        {"Lifestyle modification", 0.88},
        {"ACE inhibitor or ARB therapy when appropriate", 0.88},
        {"Thiazide-type diuretic therapy", 0.86},
        {"Calcium-channel blocker therapy", 0.86},
    }, 61, 14},

    // T16 deliberately overlaps T06. Its lower priority (84 vs. 86) means
    // T06 must execute first even though T16 has a higher computed rule CF.
    // The repeated diuretic conclusion also forces CF_combined to execute.
    {"T16", "Heart Failure", {
        {"Diuretic therapy for fluid congestion", 0.99},
    }, 84, 15},

    // T17 deliberately ties T05 on priority 88. T05 has the higher computed
    // rule CF, so the second tie-break criterion determines execution order.
    // Rate-control therapy is supported by both T05 and T17.
    {"T17", "Atrial Fibrillation", {
        {"Rate-control therapy", 0.86},
    }, 88, 16},

    // T18 adds another overlapping Stable Angina rule and preserves the
    // existing treatment terminology while giving an additional CF combination.
    {"T18", "Stable Angina", {
        {"Nitrate therapy for angina symptoms", 0.80},
        {"Antianginal therapy such as beta blockers", 0.83},
    }, 65, 17},
};

double combinePositiveCF(double cf1, double cf2) {
    return cf1 + cf2 * (1.0 - cf1);
}

bool validateCF(double cf) {
    return cf >= 0.0 && cf <= 1.0;
}

double treatmentRuleCF(double diagnosisCF, double strength) {
    double minimumPremiseCF = diagnosisCF;
    return minimumPremiseCF * strength;
}

double highestComputedRuleCF(const TreatmentRule& rule, double diagnosisCF) {
    double highest = 0.0;
    for (const auto& conclusion : rule.treatments) {
        highest = max(highest, treatmentRuleCF(diagnosisCF, conclusion.strength));
    }
    return highest;
}

vector<const TreatmentRule*> applicableRules(
    const string& diagnosis,
    double diagnosisCF
) {
    vector<const TreatmentRule*> matches;

    for (const auto& rule : RULES) {
        if (rule.diagnosis == diagnosis) {
            matches.push_back(&rule);
        }
    }

    sort(
        matches.begin(),
        matches.end(),
        [diagnosisCF](const TreatmentRule* a, const TreatmentRule* b) {
            if (a->priority != b->priority) {
                return a->priority > b->priority;
            }

            double aCF = highestComputedRuleCF(*a, diagnosisCF);
            double bCF = highestComputedRuleCF(*b, diagnosisCF);

            if (aCF != bCF) {
                return aCF > bCF;
            }

            if (a->insertionOrder != b->insertionOrder) {
                return a->insertionOrder < b->insertionOrder;
            }

            return a->ruleId < b->ruleId;
        }
    );

    return matches;
}

vector<pair<string, double>> runForwardChaining(
    const string& diagnosis,
    double diagnosisCF,
    bool showTrace = true
) {
    if (!validateCF(diagnosisCF)) {
        throw invalid_argument("Diagnosis certainty factor must be between 0 and 1.");
    }

    vector<const TreatmentRule*> rulesToFire =
        applicableRules(diagnosis, diagnosisCF);

    if (showTrace) {
        cout << string(80, '=') << '\n';
        cout << "FORWARD-CHAINING TREATMENT EXECUTION TRACE\n";
        cout << string(80, '=') << '\n';
        cout << "Input diagnosis : " << diagnosis << '\n';
        cout << "Diagnosis CF    : "
             << fixed << setprecision(3) << diagnosisCF << "\n\n";
    }

    if (rulesToFire.empty()) {
        if (showTrace) {
            cout << "Applicable treatment rules: none\n";
            cout << "No treatment recommendations were produced.\n";
        }
        return {};
    }

    if (showTrace) {
        cout << "Applicable treatment rules in execution order:\n";
        for (size_t i = 0; i < rulesToFire.size(); ++i) {
            const TreatmentRule& rule = *rulesToFire[i];
            cout << "  " << (i + 1) << ". "
                 << rule.ruleId
                 << " | priority=" << rule.priority
                 << " | highest computed rule CF="
                 << fixed << setprecision(3)
                 << highestComputedRuleCF(rule, diagnosisCF)
                 << " | insertion order="
                 << rule.insertionOrder
                 << '\n';
        }
        cout << '\n';
    }

    map<string, double> treatmentCFs;
    map<string, int> treatmentFirstSeen;
    int seenCounter = 0;

    for (const TreatmentRule* rulePtr : rulesToFire) {
        const TreatmentRule& rule = *rulePtr;

        if (showTrace) {
            cout << string(80, '-') << '\n';
            cout << "FIRING RULE " << rule.ruleId << '\n';
            cout << "Diagnosis premise : " << rule.diagnosis << '\n';
            cout << "Rule priority     : " << rule.priority << '\n';
            cout << "Insertion order   : " << rule.insertionOrder << '\n';
            cout << "Premise CFs       : ["
                 << fixed << setprecision(3)
                 << diagnosisCF << "]\n";
            cout << "Minimum premise CF: "
                 << fixed << setprecision(3)
                 << diagnosisCF << "\n\n";
        }

        for (const auto& conclusion : rule.treatments) {
            double minPremiseCF = diagnosisCF;
            double ruleCF = minPremiseCF * conclusion.strength;

            if (showTrace) {
                cout << "Treatment conclusion: " << conclusion.name << '\n';
                cout << "  Rule strength for this conclusion: "
                     << fixed << setprecision(3)
                     << conclusion.strength << '\n';
                cout << "  Rule CF = min("
                     << fixed << setprecision(3)
                     << diagnosisCF << ") x "
                     << conclusion.strength
                     << " = " << ruleCF << '\n';
            }

            auto existing = treatmentCFs.find(conclusion.name);

            if (existing == treatmentCFs.end()) {
                treatmentCFs[conclusion.name] = ruleCF;
                treatmentFirstSeen[conclusion.name] = seenCounter++;

                if (showTrace) {
                    cout << "  First support -> combined CF = "
                         << fixed << setprecision(3)
                         << ruleCF << '\n';
                }
            } else {
                double oldCF = existing->second;
                double newCF = combinePositiveCF(oldCF, ruleCF);
                treatmentCFs[conclusion.name] = newCF;

                if (showTrace) {
                    cout << "  Additional support -> combined CF = "
                         << fixed << setprecision(3)
                         << oldCF << " + "
                         << ruleCF << " x (1 - "
                         << oldCF << ") = "
                         << newCF << '\n';
                }
            }

            if (showTrace) {
                cout << '\n';
            }
        }
    }

    vector<pair<string, double>> finalResults(
        treatmentCFs.begin(),
        treatmentCFs.end()
    );

    sort(
        finalResults.begin(),
        finalResults.end(),
        [&treatmentFirstSeen](
            const pair<string, double>& a,
            const pair<string, double>& b
        ) {
            if (a.second != b.second) {
                return a.second > b.second;
            }

            int aOrder = treatmentFirstSeen.at(a.first);
            int bOrder = treatmentFirstSeen.at(b.first);

            if (aOrder != bOrder) {
                return aOrder < bOrder;
            }

            return a.first < b.first;
        }
    );

    if (showTrace) {
        cout << string(80, '=') << '\n';
        cout << "FINAL TREATMENT RECOMMENDATIONS\n";
        cout << string(80, '=') << '\n';

        for (size_t i = 0; i < finalResults.size(); ++i) {
            cout << (i + 1) << ". "
                 << finalResults[i].first
                 << " | CF="
                 << fixed << setprecision(3)
                 << finalResults[i].second
                 << '\n';
        }

        cout << '\n';
    }

    return finalResults;
}

void printDiagnosisList() {
    cout << "\nStored backward-chaining diagnosis facts:\n";

    for (size_t i = 0; i < BACKWARD_CHAINING_FACTS.size(); ++i) {
        cout << setw(2) << right << (i + 1)
             << ". "
             << left << setw(35)
             << BACKWARD_CHAINING_FACTS[i].first
             << " CF="
             << fixed << setprecision(3)
             << BACKWARD_CHAINING_FACTS[i].second
             << '\n';
    }
}

int main() {
    cout << string(80, '=') << '\n';
    cout << "CARDIOVASCULAR FORWARD-CHAINING TREATMENT SYSTEM\n";
    cout << string(80, '=') << '\n';
    cout << "Educational expert-system demonstration only.\n\n";

    while (true) {
        cout << "Options:\n";
        cout << "1. Use a stored backward-chaining diagnosis fact\n";
        cout << "2. Enter a diagnosis and diagnosis CF manually\n";
        cout << "3. Run all 15 stored diagnosis facts\n";
        cout << "0. Exit\n";
        cout << "\nEnter choice: ";

        int choice;
        cin >> choice;

        if (!cin) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice.\n\n";
            continue;
        }

        cin.ignore(10000, '\n');

        if (choice == 0) {
            cout << "Exiting.\n";
            break;
        }

        if (choice == 1) {
            printDiagnosisList();

            cout << "\nSelect diagnosis number (1-15): ";
            int selection;
            cin >> selection;

            if (!cin ||
                selection < 1 ||
                selection > static_cast<int>(BACKWARD_CHAINING_FACTS.size())) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid selection.\n\n";
                continue;
            }

            cin.ignore(10000, '\n');
            const auto& fact = BACKWARD_CHAINING_FACTS[selection - 1];
            runForwardChaining(fact.first, fact.second, true);
        }
        else if (choice == 2) {
            printDiagnosisList();

            cout << "\nEnter diagnosis exactly as listed: ";
            string diagnosis;
            getline(cin, diagnosis);

            cout << "Enter diagnosis CF (0-1): ";
            double cf;
            cin >> cf;

            if (!cin || !validateCF(cf)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Diagnosis CF must be between 0 and 1.\n\n";
                continue;
            }

            cin.ignore(10000, '\n');
            runForwardChaining(diagnosis, cf, true);
        }
        else if (choice == 3) {
            for (const auto& fact : BACKWARD_CHAINING_FACTS) {
                runForwardChaining(fact.first, fact.second, true);
            }
        }
        else {
            cout << "Invalid choice.\n\n";
        }
    }

    return 0;
}
