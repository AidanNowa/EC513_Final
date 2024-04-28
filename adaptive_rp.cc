#include "adaptive_rp.hh"
#include "mem/cache/replacement_policies/brrip_rp.hh"
#include <algorithm>      
#include <numeric>
#include <memory>
#include <typeinfo>

#define RRPV_THRESHOLD       // This needs to be modified

namespace gem5
{
namespace replacement_policy
{

AdaptiveRP(const Params &p) : BRRIP(p)
{
    BRRIP static_bbrip(p),
}

AdaptiveRP::AdaptiveRP(unsigned size) : cacheSize(size){
    currentPolicy = std::make_unique<SRRIP>(size);
}

void AdaptiveRP::evaluatePolicy(){
    auto rrpv_values = currentPolicy->getRRPVs();

    if (rrpv_values.empty())
        return;

    // Calculate average RRPV value
    double average_rrpv = std::accumulate(rrpv_values.begin(), rrpv_values.end(), 0.0) / rrpv_values.size();

    // Select which policy to use based on threshold
    // TODO: Threshold should be based on cache size
    if(average_rrpv > RRPV_THRESHOLD && typid(*currentPolicy) != typeid(BRRIP)) {
        currentPolicy = std::make_unique<BRRIP>();
    }else if(average_rrpv <= RRPV_THRESHOLD && typeid(*currentPolicy) != typeid(SRRIP)){
        currentPolicy = std::make_unique<SRRIP>();
    }

}

//function that differs from brrip_rp that selects between which btp to utilize 
void
AdaptiveRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    std::shared_ptr<BRRIPReplData> casted_replacement_data =
        std::static_pointer_cast<BRRIPReplData>(replacement_data);

    //add case here to select between btp=100(static) or btp=3()

    // Reset RRPV
    // Replacement data is inserted as "long re-reference" if lower than btp,
    // "distant re-reference" otherwise
    casted_replacement_data->rrpv.saturate();
    if (random_mt.random<unsigned>(1, 100) <= btp) {
        casted_replacement_data->rrpv--;
    }

    // Mark entry as ready to be used
    casted_replacement_data->valid = true;
}



} // namespace replacement_policy
} // namespace gem5