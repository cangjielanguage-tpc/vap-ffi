package ohos_app_cangjie_vap_cj_test

internal import ohos.ability.AbilityStage

class MyAbilityStage <: AbilityStage {
    public override func onCreate(): Unit {
        AppLog.info("MyAbilityStage onCreated.")
    }
}
