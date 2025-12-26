#include <wx/wx.h>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <vector>

const std::unordered_set<std::string> commonPasswords = { //Set for O(1) lookup
    "123456", "password", "123456789", "12345678", "12345",
    "qwerty", "abc123", "abc", "mypassword", "123123",
    "iloveyou", "passw0rd", "1234"
};

class MyFrame : public wxFrame {
public:
    wxTextCtrl* inputField;
    wxStaticText* resultText;
    wxStaticText* tipText;

    MyFrame() : wxFrame(NULL, wxID_ANY, "Password strength checker", wxDefaultPosition, wxSize(400, 300)) {
        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        inputField = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
        resultText = new wxStaticText(panel, wxID_ANY, "");
        tipText = new wxStaticText(panel, wxID_ANY, "");
        tipText->SetForegroundColour(*wxRED); // Colour tips

        wxButton* checkBtn = new wxButton(panel, wxID_ANY, "Check");

        sizer->Add(inputField, 0, wxEXPAND | wxALL, 10);
        sizer->Add(checkBtn, 0, wxEXPAND | wxALL, 10);
        sizer->Add(resultText, 0, wxEXPAND | wxALL, 10);
        sizer->Add(tipText, 0, wxEXPAND | wxALL, 10);

        panel->SetSizer(sizer);
        checkBtn->Bind(wxEVT_BUTTON, &MyFrame::onCheck, this);
    }

    void onCheck(wxCommandEvent&) {
        std::string password = inputField->GetValue().ToStdString();
        std::vector<std::string> tips;

        if (password.empty()) return;

        // checking if in set
        if (commonPasswords.find(password) != commonPasswords.end()) {
            resultText->SetLabel("Score: 0");
            tipText->SetLabel("Tip: This is a very common password. Change it!");
            return;
        }

        // checking for repeat in password (logner than 2)
        bool hasRepeatingPattern = false;
        if (password.length() >= 4) {
            for (size_t i = 2; i <= password.length() / 2; i++) {
                std::string sub = password.substr(0, i);
                if (password.find(sub, i) != std::string::npos) {
                    hasRepeatingPattern = true;
                    break;
                }
            }
        }

        // adding score for capitals, numbers and symbols
        int score = (int)std::min(password.length(), (size_t)15) * 5;

        bool hasUpper = std::any_of(password.begin(), password.end(), ::isupper);
        bool hasDigit = std::any_of(password.begin(), password.end(), ::isdigit);
        bool hasSymbol = std::any_of(password.begin(), password.end(), [](char c) { return !isalnum(c); });

        if (hasUpper) score += 10; else tips.push_back("Add capital letters");
        if (hasDigit) score += 10; else tips.push_back("Add numbers");
        if (hasSymbol) score += 15; else tips.push_back("Add symbols (@, #, etc.)");

        if (hasRepeatingPattern) {
            score -= 20;
            tips.push_back("Avoid repeating words/patterns");
        }
        if (password.length() < 8) tips.push_back("Make it longer (8+ chars)");

        // update UI
        resultText->SetLabel(wxString::Format("Score: %d", std::max(0, score)));

        // show tip
        if (tips.empty()) {
            tipText->SetLabel("Password looks secure!");
            tipText->SetForegroundColour(wxColour(0, 150, 0)); // Green
        }
        else {
            tipText->SetLabel("Tip: " + tips[0]);
            tipText->SetForegroundColour(*wxRED);
        }
    }
};

class MyApp : public wxApp { public: virtual bool OnInit() { (new MyFrame())->Show(); return true; } };
wxIMPLEMENT_APP(MyApp);
