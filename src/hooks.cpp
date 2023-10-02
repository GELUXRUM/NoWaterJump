#include "hooks.h"
#include "detourxs/detourxs.h"

namespace HookLineAndSinker
{
	typedef void(JumpHandler_HandleEventSig)(RE::JumpHandler*, RE::ButtonEvent*);
	REL::Relocation<JumpHandler_HandleEventSig> OriginalFunction;
	DetourXS hook;

	void HookedJumpHandler_HandleEvent(RE::JumpHandler* a_jumpHandler, RE::ButtonEvent* a_buttonEvent)
	{
		if ((static_cast<uint32_t>(playerRef->currentProcess->middleHigh->charController->context.currentState.get()) & static_cast<uint32_t>(RE::hknpCharacterState::hknpCharacterStateType::kSwimming)) == false ) {
			OriginalFunction(a_jumpHandler, a_buttonEvent);
			return;
		} else {
			/* while it works without this, I think it's still better to include it
			 * as it's always set to 1 in the original function before the function
			 * returns regardless of whether a jump happened or not */
			a_buttonEvent->handled.set(RE::ButtonEvent::HANDLED_RESULT::kContinue);
			return;
		}
	}

	void RegisterHook()
	{
		REL::Relocation<JumpHandler_HandleEventSig> funcLocation{ REL::ID(882498) };
		if (hook.Create(reinterpret_cast<LPVOID>(funcLocation.address()), &HookedJumpHandler_HandleEvent)) {
			OriginalFunction = reinterpret_cast<std::uintptr_t>(hook.GetTrampoline());
		} else {
			logger::warn("Failed to create hook");
		}
	}
}
