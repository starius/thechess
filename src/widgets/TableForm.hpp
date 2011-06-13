#ifndef THECHESS_WIDGETS_TABLE_FORM_HPP_
#define THECHESS_WIDGETS_TABLE_FORM_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTable>
#include <Wt/WString>
#include <Wt/WWidget>
#include <Wt/WFormWidget>

namespace thechess {
namespace widgets {

class TableForm : public Wt::WTable
{
public:
    TableForm(Wt::WContainerWidget* parent=0);
    void section(const Wt::WString& header);
    Wt::WContainerWidget* item(const Wt::WString& name,
        const Wt::WString& description="",
        Wt::WFormWidget* fw=0, Wt::WWidget* input=0, bool row=true);
};

}
}


#endif

