/* ============================================================
 * 🧩 ProductValidation – domain-level validation implementation
 * ============================================================ */
#include "products/validation/product_validation.h"

#include "barcodes/validator/barcode_validator.h"
#include "products/model/product_master.h"

#include <products/registry/product_registry.h>

namespace ProductValidation {

bool validateName(const QString& name,
                  const QUuid& parent_id,
                  QString& out_error)
{
    const QString trimmed = name.trimmed();

    if (trimmed.isEmpty()) {
        out_error = "A név nem lehet üres.";
        return false;
    }

    // Duplikáció ellenőrzése parentId alatt
    bool exists = ProductRegistry::instance().existsBy([&](const ProductMaster& p){
        return p.parentId == parent_id
               && p.name.compare(trimmed, Qt::CaseInsensitive) == 0;
    });

    if (exists) {
        out_error = "Már létezik ilyen nevű elem ezen a szinten.";
        return false;
    }

    return true;
}


bool validateBarcode_UI_DryRun(const QString& code,
                               const QString& entity_type,
                               const QUuid& id,
                               const QString& name,
                               QString& out_error)
{
    return BarcodeValidator::validate_UI(code,
                                         entity_type,
                                         id,
                                         name,
                                         out_error);
}

bool registerBarcode_UI(const QString& code,
                        const QString& entity_type,
                        const QUuid& id,
                        const QString& name,
                        QString& out_error)
{
    return BarcodeValidator::checkAndRegister_UI(code,
                                                 entity_type,
                                                 id,
                                                 name,
                                                 out_error);
}



} // namespace ProductValidation
