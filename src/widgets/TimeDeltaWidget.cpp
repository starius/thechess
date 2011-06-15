#include <Wt/WContainerWidget>
#include <Wt/WComboBox>

#include "widgets/TimeDeltaWidget.hpp"
#include "widgets/MySpinBox.hpp"
#include "time_intervals.hpp"

namespace thechess {
namespace widgets {

const int intervals_size = 5;
const Td intervals[5] = {second, minute, hour, day, week};

class TimeDeltaWidgetImpl : public Wt::WContainerWidget
{
public:
    TimeDeltaWidgetImpl(const Td& min, const Td& value, const Td& max) :
    Wt::WContainerWidget(), min_(min), max_(max), interval_(second)
    {
        setInline(true);
        spin_box_ = new MyDoubleSpinBox(this);
        spin_box_->setRange(min.total_seconds(), max.total_seconds());
        spin_box_->setValue(value.total_seconds());
        combo_box_ = new Wt::WComboBox(this);
        combo_box_->addItem(tr("thechess.seconds"));
        combo_box_->addItem(tr("thechess.minutes"));
        combo_box_->addItem(tr("thechess.hours"));
        combo_box_->addItem(tr("thechess.days"));
        combo_box_->addItem(tr("thechess.weeks"));
        combo_box_->activated()
            .connect(this, &TimeDeltaWidgetImpl::select_handler_);
        for (int i = intervals_size - 1; i >= 0; i--)
        {
            if (value >= intervals[i])
            {
                combo_box_->setCurrentIndex(i);
                select_handler_(i);
                break;
            }
        }
    }

    Td value() const
    {
        return spin_box_->value() * intervals[combo_box_->currentIndex()];
    }

    Wt::WFormWidget* form_widget()
    {
        return spin_box_;
    }

private:
    MyDoubleSpinBox* spin_box_;
    Wt::WComboBox* combo_box_;
    Td min_;
    Td max_;
    Td interval_;

    void select_handler_(int i)
    {
        Td s = spin_box_->value() * interval_;
        interval_ = intervals[i];
        spin_box_->setRange(min_ / interval_, max_ / interval_);
        spin_box_->setValue(s / interval_);
    }
};


TimeDeltaWidget::TimeDeltaWidget(const Td& min, const Td& value,
    const Td& max, Wt::WContainerWidget* parent) :
WCompositeWidget(parent)
{
    impl_ = new TimeDeltaWidgetImpl(min, value, max);
    setImplementation(impl_);
}

Td TimeDeltaWidget::value() const
{
    return impl_->value();
}


Wt::WFormWidget* TimeDeltaWidget::form_widget()
{
    return impl_->form_widget();
}

}
}

