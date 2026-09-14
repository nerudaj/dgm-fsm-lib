// @ts-check

/** @type {Program|null} */
let program = null;

/**
 * 
 * @param {(file: File) => void} callback 
 */
function selectFileAndExecute(callback) {
    const input = document.createElement("input");
    input.type = "file";
    input.accept = ".json,application/json";

    input.addEventListener("change", function () {
        const file = input.files && input.files[0] ? input.files[0] : null;

        if (!file) {
            console.error("No file selected.");
            return;
        }

        callback(file);
    });

    input.click();
}

function loadManifest() {
    selectFileAndExecute((file) => {
        if (program) {
            program.loadManifestFromFile(file);
        }
    });
}

function loadModel() {
    selectFileAndExecute((file) => {
        if (program) {
            program.loadModelFromFile(file);
        }
    });
}

function saveModel() {
    selectFileAndExecute((file) => {
        if (program) {
            program.saveModelToFile(file);
        }
    });
}

function undo() {
    if (program) {
        program.undo();
    }
}

function redo() {
    if (program) {
        program.redo();
    }
}

/**
 * @param {any} e 
 */
function addState(e) {
    console.log("addState:begin");
    var stateNameInput = document.getElementById("AddState_NameInput");
    var actionNameSelect = document.getElementById("AddState_ActionInput");

    if (!program) {
        console.error("Program is null");
        return;
    }
    else if (!stateNameInput || !(stateNameInput instanceof HTMLInputElement)) {
        console.error("State name input is either null or wrong type");
        return;
    }
    else if (!actionNameSelect || !(actionNameSelect instanceof HTMLSelectElement)) {
        console.error("Action name input is either null or wrong type");
        return;
    }

    program.addNewState(
        stateNameInput.value,
        actionNameSelect.value);
    console.log("addState:end");
}

/**
 * @param {number} transitionIdx 
 */
function openAddTransitionModal(transitionIdx) {
    if (!program) {
        console.error("Program is null");
        return;
    }

    var condSelect = document.getElementById("EditTransition_ConditionSelect");
    var destSelect = document.getElementById("EditTransition_DestinationSelect");
    var indexInput = document.getElementById("EditTransition_Index");

    if (!condSelect || !(condSelect instanceof HTMLSelectElement)) {
        console.error("Condition select is either null or wrong type");
        return;
    }
    else if (!destSelect || !(destSelect instanceof HTMLSelectElement)) {
        console.error("Destination select is either null or wrong type");
        return;
    }
    else if (!indexInput || !(indexInput instanceof HTMLInputElement)) {
        console.error("Index input is either null or wrong type");
        return;
    }

    DomHelper.populateSelectElement(
        condSelect,
        program.ir.manifest.conditionNames.map(condName => ({ value: condName, label: condName })));
    DomHelper.populateSelectElement(
        destSelect,
        Object.entries(program.getCurrentStates()).map(([state, ir]) => ({ value: state, label: ir.name })));

    indexInput.value = transitionIdx !== null ? transitionIdx.toString() : "";

    showModal("EditTransitionModal");
}

function addTransitionSelection() {
    if (!program) return;

    var dom = document.getElementById("EditState_TransitionList");

    var li = document.createElement("li");
    li.className = "list-group-item";

    var conditionSelect = document.createElement("select");
    conditionSelect.className = "form-select mb-2 mb-md-0"; // simple stacking on small screens
    conditionSelect.id = "EditState_TransitionFromSelect_" + Date.now();

    var destinationSelect = document.createElement("select");
    destinationSelect.className = "form-select";
    destinationSelect.id = "EditState_TransitionToSelect_" + Date.now();

    DomHelper.populateSelectElement(conditionSelect, program.ir.manifest.conditionNames.map(x => ({ value: x, label: x })));
    DomHelper.populateSelectElement(destinationSelect, program.getStateNamesInCurrentMachine());

    // Layout wrapper (so the selects sit next to each other with "->" between)
    var row = document.createElement("div");
    row.className = "row g-2 align-items-center";

    var colFrom = document.createElement("div");
    colFrom.className = "col-12 col-md-5";
    colFrom.appendChild(conditionSelect);

    var colArrow = document.createElement("div");
    colArrow.className = "col-12 col-md-2 text-center";
    colArrow.style.userSelect = "none";
    colArrow.textContent = "->";

    var colTo = document.createElement("div");
    colTo.className = "col-12 col-md-5";
    colTo.appendChild(destinationSelect);

    row.appendChild(colFrom);
    row.appendChild(colArrow);
    row.appendChild(colTo);

    // Add to li and append to DOM
    li.appendChild(row);
    dom.appendChild(li);
}

/**
 * @param {any} e 
 */
function saveTransition(e) {
    if (!program) {
        console.error("Program is null");
        return;
    }
}

/**
 * @param {any} event 
 */
function onEditStateNameChange(event) {
    if (!program) {
        console.error("Program is null");
        return;
    }

    program.onSelectedStateNameChange(event.target.value);
}

/**
 * @param {any} event
 */
function onEditStateActionChange(event) {
    if (!program) {
        console.error("Program is null");
        return;
    }

    program.onSelectedStateActionChange(event.target.value);
}

/**
 * @param {any} event
 */
// TODO: probably no longer used
function onEditStateDestinationSelect(event) {
    if (!program) {
        console.error("Program is null");
        return;
    }

    program.onSelectedStateDestinationChange(event.target.value);
}

function applyChangesToState() {
    if (program) {
        program.updateSelectedState();
    }
}

/**
 * @param {string} id 
 */
function showModal(id) {
    const modalElement = document.getElementById(id);
    if (!modalElement) {
        console.error("Modal element not found.");
        return;
    }

    const modal = bootstrap.Modal.getOrCreateInstance(modalElement);
    modal.show();
}

function main() {
    console.log("main started");

    program = new Program();

    console.log("main finished");
}
